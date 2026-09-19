#include "../GlobalShortcut.h"

#include <QDebug>
#include <QKeyCombination>
#include <QKeySequence>
#include <QSocketNotifier>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <algorithm>


namespace
{
    /*
     * X11 display used by the global shortcut.
     */
    Display *g_display = nullptr;

    /*
     * Root window on which the shortcut is registered.
     */
    Window g_rootWindow = None;

    /*
     * X11 keycode for the configured key.
     *
     * Example:
     *
     *     V -> V keycode
     */
    int g_keycode = 0;

    /*
     * X11 modifier mask for the configured shortcut.
     *
     * Example:
     *
     *     Ctrl + Shift
     */
    unsigned int g_modifiers = 0;

    /*
     * Whether a shortcut is currently registered.
     */
    bool g_registered = false;


    /**
     * @brief Converts Qt keyboard modifiers to X11 modifiers.
     */
    unsigned int qtModifiersToX11(
        Qt::KeyboardModifiers modifiers
    )
    {
        unsigned int x11Modifiers = 0;

        if (
            modifiers &
            Qt::ControlModifier
        )
        {
            x11Modifiers |= ControlMask;
        }

        if (
            modifiers &
            Qt::ShiftModifier
        )
        {
            x11Modifiers |= ShiftMask;
        }

        if (
            modifiers &
            Qt::AltModifier
        )
        {
            x11Modifiers |= Mod1Mask;
        }

        /*
         * Qt::MetaModifier normally represents the Super/Windows
         * key on Linux.
         */
        if (
            modifiers &
            Qt::MetaModifier
        )
        {
            x11Modifiers |= Mod4Mask;
        }

        return x11Modifiers;
    }


    /**
     * @brief Converts a Qt key into an X11 keysym.
     *
     * This covers the common keys useful for a global shortcut.
     */
    KeySym qtKeyToX11KeySym(
        Qt::Key key
    )
    {
        /*
         * ---------------------------------------------------------
         * Alphabetic keys
         * ---------------------------------------------------------
         *
         * Qt::Key_A ... Qt::Key_Z use the same numeric values
         * as the ASCII uppercase characters.
         */
        if (
            key >= Qt::Key_A &&
            key <= Qt::Key_Z
        )
        {
            return static_cast<KeySym>(
                key
            );
        }

        /*
         * ---------------------------------------------------------
         * Number keys
         * ---------------------------------------------------------
         */
        if (
            key >= Qt::Key_0 &&
            key <= Qt::Key_9
        )
        {
            return static_cast<KeySym>(
                key
            );
        }

        /*
         * ---------------------------------------------------------
         * Function keys
         * ---------------------------------------------------------
         */
        if (
            key >= Qt::Key_F1 &&
            key <= Qt::Key_F35
        )
        {
            return XK_F1 +
                (
                    static_cast<int>(key)
                    -
                    static_cast<int>(Qt::Key_F1)
                );
        }

        /*
         * ---------------------------------------------------------
         * Common special keys
         * ---------------------------------------------------------
         */
        switch (key)
        {
            case Qt::Key_Space:
                return XK_space;

            case Qt::Key_Return:
                return XK_Return;

            case Qt::Key_Enter:
                return XK_KP_Enter;

            case Qt::Key_Tab:
                return XK_Tab;

            case Qt::Key_Backspace:
                return XK_BackSpace;

            case Qt::Key_Escape:
                return XK_Escape;

            case Qt::Key_Delete:
                return XK_Delete;

            case Qt::Key_Insert:
                return XK_Insert;

            case Qt::Key_Home:
                return XK_Home;

            case Qt::Key_End:
                return XK_End;

            case Qt::Key_PageUp:
                return XK_Page_Up;

            case Qt::Key_PageDown:
                return XK_Page_Down;

            case Qt::Key_Left:
                return XK_Left;

            case Qt::Key_Right:
                return XK_Right;

            case Qt::Key_Up:
                return XK_Up;

            case Qt::Key_Down:
                return XK_Down;

            default:
                break;
        }

        /*
         * Unsupported key.
         */
        return NoSymbol;
    }
}


/**
 * @brief Creates the global shortcut object.
 */
GlobalShortcut::GlobalShortcut(
    QObject *parent
)
    : QObject(parent),
      m_shortcut(),
      m_socketNotifier(nullptr)
{
}


/**
 * @brief Destroys the global shortcut.
 */
GlobalShortcut::~GlobalShortcut()
{
    unregisterShortcut();
}


/**
 * @brief Registers a configurable global shortcut.
 */
bool GlobalShortcut::registerShortcut(
    const QString &shortcut
)
{
    /*
     * If a shortcut is already registered, do not register
     * another one on top of it.
     */
    if (g_registered)
    {
        return true;
    }

    /*
     * Convert the stored Qt string into a QKeySequence.
     */
    const QKeySequence sequence =
        QKeySequence::fromString(
            shortcut,
            QKeySequence::PortableText
        );

    /*
     * A global shortcut must contain exactly one key
     * combination.
     */
    if (
        sequence.isEmpty()
        ||
        sequence.count() != 1
    )
    {
        qWarning()
            << "Clipo X11:"
            << "Invalid global shortcut:"
            << shortcut;

        return false;
    }

    /*
     * Extract the first key combination.
     */
    const QKeyCombination combination =
        sequence[0];

    const Qt::Key key =
        combination.key();

    const Qt::KeyboardModifiers modifiers =
        combination.keyboardModifiers();

    /*
     * A shortcut must contain at least one modifier.
     *
     * Registering a plain "V" globally would interfere with
     * normal keyboard input.
     */
    if (
        modifiers == Qt::NoModifier
    )
    {
        qWarning()
            << "Clipo X11:"
            << "Global shortcut must contain a modifier:"
            << shortcut;

        return false;
    }

    /*
     * Convert the Qt key to an X11 keysym.
     */
    const KeySym keySym =
        qtKeyToX11KeySym(
            key
        );

    if (
        keySym == NoSymbol
    )
    {
        qWarning()
            << "Clipo X11:"
            << "Unsupported shortcut key:"
            << shortcut;

        return false;
    }

    /*
     * Open X11 display.
     */
    g_display =
        XOpenDisplay(nullptr);

    if (
        g_display == nullptr
    )
    {
        qWarning()
            << "Clipo X11:"
            << "Could not open X display.";

        return false;
    }

    /*
     * Get the root window.
     */
    const int screen =
        DefaultScreen(
            g_display
        );

    g_rootWindow =
        RootWindow(
            g_display,
            screen
        );

    /*
     * Convert X11 keysym to the physical keycode.
     */
    g_keycode =
        XKeysymToKeycode(
            g_display,
            keySym
        );

    if (
        g_keycode == 0
    )
    {
        qWarning()
            << "Clipo X11:"
            << "Could not find keycode for:"
            << shortcut;

        XCloseDisplay(
            g_display
        );

        g_display = nullptr;

        return false;
    }

    /*
     * Convert Qt modifiers to X11 modifiers.
     */
    g_modifiers =
        qtModifiersToX11(
            modifiers
        );

    /*
     * X11 adds extra modifier bits for Caps Lock and Num Lock.
     *
     * Register all combinations so the shortcut works regardless
     * of whether Caps Lock or Num Lock is enabled.
     */
    const unsigned int combinations[] =
    {
        0,
        LockMask,
        Mod2Mask,
        LockMask | Mod2Mask
    };

    for (
        unsigned int extraModifiers :
        combinations
    )
    {
        XGrabKey(
            g_display,
            g_keycode,
            g_modifiers |
            extraModifiers,
            g_rootWindow,
            False,
            GrabModeAsync,
            GrabModeAsync
        );
    }

    /*
     * Send all pending X11 requests immediately.
     */
    XSync(
        g_display,
        False
    );

    /*
     * Get the file descriptor associated with X11.
     */
    const int x11FileDescriptor =
        ConnectionNumber(
            g_display
        );

    /*
     * Let Qt monitor the X11 connection.
     */
    m_socketNotifier =
        new QSocketNotifier(
            x11FileDescriptor,
            QSocketNotifier::Read,
            this
        );

    connect(
        m_socketNotifier,
        &QSocketNotifier::activated,
        this,
        [this]()
        {
            processEvents();
        }
    );

    /*
     * Store the successfully registered shortcut.
     */
    m_shortcut =
        shortcut;

    g_registered = true;

    qDebug()
        << "Clipo X11:"
        << "Global shortcut registered:"
        << shortcut;

    return true;
}


/**
 * @brief Changes the active global shortcut.
 */
bool GlobalShortcut::setShortcut(
    const QString &shortcut
)
{
    /*
     * Do not do unnecessary work if the shortcut has not
     * actually changed.
     */
    if (
        g_registered &&
        m_shortcut == shortcut
    )
    {
        return true;
    }

    /*
     * Remove the old shortcut.
     */
    unregisterShortcut();

    /*
     * Register the new shortcut.
     */
    return registerShortcut(
        shortcut
    );
}


/**
 * @brief Returns the currently registered shortcut.
 */
QString GlobalShortcut::shortcut() const
{
    return m_shortcut;
}


/**
 * @brief Processes pending X11 events.
 */
void GlobalShortcut::processEvents()
{
    if (
        g_display == nullptr
    )
    {
        return;
    }

    /*
     * Process every event currently waiting on the X11
     * connection.
     */
    while (
        XPending(g_display) > 0
    )
    {
        XEvent event;

        XNextEvent(
            g_display,
            &event
        );

        /*
         * We only care about KeyPress events.
         */
        if (
            event.type != KeyPress
        )
        {
            continue;
        }

        /*
         * Check the actual keycode.
         */
        if (
            event.xkey.keycode
            !=
            static_cast<unsigned int>(
                g_keycode
            )
        )
        {
            continue;
        }

        /*
         * Ignore Caps Lock and Num Lock modifier bits.
         */
        const unsigned int modifiers =
            event.xkey.state &
            ~(LockMask | Mod2Mask);

        /*
         * Make sure the required shortcut modifiers are
         * actually pressed.
         */
        if (
            modifiers
            !=
            g_modifiers
        )
        {
            continue;
        }

        qDebug()
            << "Clipo X11:"
            << "Global shortcut detected:"
            << m_shortcut;

        /*
         * Notify the Qt application.
         */
        emit activated();
    }
}


/**
 * @brief Unregisters the current global shortcut.
 */
void GlobalShortcut::unregisterShortcut()
{
    if (
        !g_registered
        ||
        g_display == nullptr
    )
    {
        return;
    }

    /*
     * Stop monitoring the X11 socket first.
     */
    if (
        m_socketNotifier != nullptr
    )
    {
        m_socketNotifier->setEnabled(
            false
        );

        delete m_socketNotifier;

        m_socketNotifier = nullptr;
    }

    /*
     * Remove every modifier combination we registered.
     */
    const unsigned int combinations[] =
    {
        0,
        LockMask,
        Mod2Mask,
        LockMask | Mod2Mask
    };

    for (
        unsigned int extraModifiers :
        combinations
    )
    {
        XUngrabKey(
            g_display,
            g_keycode,
            g_modifiers |
            extraModifiers,
            g_rootWindow
        );
    }

    XSync(
        g_display,
        False
    );

    /*
     * Close the X11 connection.
     */
    XCloseDisplay(
        g_display
    );

    /*
     * Reset all global state.
     */
    g_display = nullptr;

    g_rootWindow = None;

    g_keycode = 0;

    g_modifiers = 0;

    g_registered = false;

    m_shortcut.clear();

    qDebug()
        << "Clipo X11:"
        << "Global shortcut unregistered.";
}
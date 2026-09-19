#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QString>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include <unistd.h>


/*
 * Stores the X11 window that was active before Clipo opened.
 *
 * Only the X11 window ID is stored.
 */
static Window g_previousActiveWindow = None;


/**
 * @brief Opens a connection to the X11 display.
 */
static Display *openX11Display()
{
    return XOpenDisplay(nullptr);
}


/**
 * @brief Gets the currently active X11 window.
 */
static Window getActiveWindow(
    Display *display
)
{
    if (display == nullptr) {
        return None;
    }


    const Window rootWindow =
        DefaultRootWindow(display);


    const Atom activeWindowAtom =
        XInternAtom(
            display,
            "_NET_ACTIVE_WINDOW",
            True
        );


    if (activeWindowAtom == None) {
        return None;
    }


    Atom actualType = None;
    int actualFormat = 0;

    unsigned long itemCount = 0;
    unsigned long bytesAfter = 0;

    unsigned char *propertyData = nullptr;


    const int result =
        XGetWindowProperty(
            display,
            rootWindow,
            activeWindowAtom,
            0,
            1,
            False,
            AnyPropertyType,
            &actualType,
            &actualFormat,
            &itemCount,
            &bytesAfter,
            &propertyData
        );


    if (
        result != Success ||
        propertyData == nullptr ||
        itemCount == 0
    ) {

        if (propertyData != nullptr) {
            XFree(propertyData);
        }

        return None;
    }


    const Window activeWindow =
        *reinterpret_cast<Window *>(
            propertyData
        );


    XFree(propertyData);


    return activeWindow;
}


/**
 * @brief Remembers the currently active application.
 */
bool rememberActiveWindowX11()
{
    Display *display =
        openX11Display();


    if (display == nullptr) {

        qDebug()
            << "Clipo X11:"
            << "Could not open X11 display.";

        return false;
    }


    g_previousActiveWindow =
        getActiveWindow(display);


    qDebug()
        << "Clipo X11:"
        << "Remembered window:"
        << Qt::hex
        << static_cast<unsigned long>(
               g_previousActiveWindow
           );


    XCloseDisplay(display);


    return (
        g_previousActiveWindow
        != None
    );
}


/**
 * @brief Requests activation of the previously active window.
 */
static bool activatePreviousWindow(
    Display *display
)
{
    if (
        display == nullptr ||
        g_previousActiveWindow == None
    ) {
        return false;
    }


    const Window rootWindow =
        DefaultRootWindow(display);


    const Atom activeWindowAtom =
        XInternAtom(
            display,
            "_NET_ACTIVE_WINDOW",
            False
        );


    if (activeWindowAtom == None) {
        return false;
    }


    XEvent event{};


    event.xclient.type =
        ClientMessage;

    event.xclient.serial =
        0;

    event.xclient.send_event =
        True;

    event.xclient.message_type =
        activeWindowAtom;

    event.xclient.window =
        g_previousActiveWindow;

    event.xclient.format =
        32;


    /*
     * EWMH:
     *
     * data[0] = source indication
     * data[1] = timestamp
     * data[2] = current active window
     */
    event.xclient.data.l[0] =
        2;

    event.xclient.data.l[1] =
        CurrentTime;

    event.xclient.data.l[2] =
        g_previousActiveWindow;

    event.xclient.data.l[3] =
        0;

    event.xclient.data.l[4] =
        0;


    const Status result =
        XSendEvent(
            display,
            rootWindow,
            False,
            SubstructureRedirectMask |
            SubstructureNotifyMask,
            &event
        );


    XFlush(display);


    qDebug()
        << "Clipo X11:"
        << "Activation request:"
        << (result != 0);


    return result != 0;
}


/**
 * @brief Checks whether the previously active window is now
 *        the active X11 window.
 */
static bool isPreviousWindowActive(
    Display *display
)
{
    if (
        display == nullptr ||
        g_previousActiveWindow == None
    ) {
        return false;
    }


    return (
        getActiveWindow(display)
        == g_previousActiveWindow
    );
}


/**
 * @brief Sends Ctrl+V through XTest.
 */
static bool sendPasteKey(
    Display *display
)
{
    if (display == nullptr) {
        return false;
    }


    const KeyCode vKey =
        XKeysymToKeycode(
            display,
            XK_V
        );


    const KeyCode ctrlKey =
        XKeysymToKeycode(
            display,
            XK_Control_L
        );


    if (
        vKey == 0 ||
        ctrlKey == 0
    ) {

        qDebug()
            << "Clipo X11:"
            << "Could not resolve keyboard keycodes.";

        return false;
    }


    /*
     * Ctrl down.
     */
    XTestFakeKeyEvent(
        display,
        ctrlKey,
        True,
        CurrentTime
    );


    /*
     * V down.
     */
    XTestFakeKeyEvent(
        display,
        vKey,
        True,
        CurrentTime
    );


    /*
     * V up.
     */
    XTestFakeKeyEvent(
        display,
        vKey,
        False,
        CurrentTime
    );


    /*
     * Ctrl up.
     */
    XTestFakeKeyEvent(
        display,
        ctrlKey,
        False,
        CurrentTime
    );


    XFlush(display);


    qDebug()
        << "Clipo X11:"
        << "Ctrl+V sent.";


    return true;
}


/**
 * @brief Copies text to the clipboard, restores the previous
 * application and sends Ctrl+V.
 */
bool pasteUsingX11(
    const QString &text
)
{
    if (text.isEmpty()) {

        qDebug()
            << "Clipo X11:"
            << "Empty text.";

        return false;
    }


    if (
        g_previousActiveWindow
        == None
    ) {

        qDebug()
            << "Clipo X11:"
            << "No previous window.";

        return false;
    }


    qDebug()
        << "Clipo X11:"
        << "Paste requested.";


    /*
     * Put selected text into the system clipboard.
     */
    QClipboard *clipboard =
        QApplication::clipboard();


    clipboard->setText(
        text
    );


    QApplication::processEvents();


    /*
     * Open X11.
     */
    Display *display =
        openX11Display();


    if (display == nullptr) {

        qDebug()
            << "Clipo X11:"
            << "Could not open display.";

        return false;
    }


    /*
     * Restore previous application.
     */
    if (
        !activatePreviousWindow(
            display
        )
    ) {

        qDebug()
            << "Clipo X11:"
            << "Activation request failed.";

        XCloseDisplay(display);

        return false;
    }


    /*
     * Wait for the window manager to activate it.
     *
     * Maximum wait:
     *
     *     50 × 10 ms = 500 ms
     */
    constexpr int maxAttempts = 50;

    bool active = false;


    for (
        int attempt = 0;
        attempt < maxAttempts;
        ++attempt
    ) {

        if (
            isPreviousWindowActive(
                display
            )
        ) {

            active = true;

            break;
        }


        usleep(10000);


        QApplication::processEvents();
    }


    qDebug()
        << "Clipo X11:"
        << "Previous window active:"
        << active;


    if (!active) {

        qDebug()
            << "Clipo X11:"
            << "Previous application did not regain focus.";

        XCloseDisplay(display);

        return false;
    }


    /*
     * Give the application a tiny amount of time to receive
     * keyboard focus.
     */
    usleep(20000);


    QApplication::processEvents();


    /*
     * Generate Ctrl+V.
     */
    const bool result =
        sendPasteKey(display);


    XCloseDisplay(display);


    return result;
}


/**
 * @brief Clears the remembered X11 window.
 */
void clearActiveWindowX11()
{
    g_previousActiveWindow =
        None;


    qDebug()
        << "Clipo X11:"
        << "Previous window cleared.";
}
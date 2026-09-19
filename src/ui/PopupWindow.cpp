#include "PopupWindow.h"
#include "../settings/SettingsManager.h"
#include "../settings/SettingsDialog.h"

#include <QFile>
#include <QPalette>
#include <QTextStream>
#include "ClipboardHistoryView.h"

#include "../database/Database.h"
#include "../settings/SettingsManager.h"
#include "../settings/SettingsDialog.h"
#include "../platform/PasteController.h"

#include <QApplication>
#include <QCursor>
#include <QDebug>
#include <QDialog>
#include <QEvent>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QScreen>
#include <QVBoxLayout>




/**
 * @brief Creates the Clipo popup window.
 *
 * The popup itself does not own the database.
 * Database is created and managed by the application,
 * and a reference is passed here.
 */
PopupWindow::PopupWindow(
    Database &database,
    SettingsManager &settings,
    QWidget *parent
)
    : QWidget(parent),
      m_database(database),
      m_settings(settings),
      m_header(nullptr),
    //   m_titleLabel(nullptr),
      m_settingsButton(nullptr),
      m_minimizeButton(nullptr),
      m_closeButton(nullptr),
      m_searchBox(nullptr),
      m_historyView(nullptr),
      m_dragOffset(),
      m_dragging(false)
{
    setObjectName("PopupWindow");

    /*
     * Allows us to paint a rounded popup with transparent
     * corners instead of using the default rectangular window.
     */
    setAttribute(
        Qt::WA_TranslucentBackground
    );

    /*
     * Allows Qt stylesheets to work correctly with
     * the custom painted background.
     */
    setAttribute(
        Qt::WA_StyledBackground,
        true
    );

    /*
     * Create all UI widgets.
     */
    setupUi();

    /*
     * Apply the Clipo visual theme.
     */
    setupStyle();
}


/**
 * @brief Paints the rounded popup background.
 *
 * The popup uses an approximately 88% opaque dark background.
 * This gives Clipo a modern translucent appearance while
 * keeping the clipboard text easy to read.
 */
/**
 * @brief Paints the rounded Clipo popup background.
 *
 * The background color depends on the currently selected theme.
 *
 * Light mode:
 *     Soft white translucent background.
 *
 * Dark mode:
 *     Soft charcoal translucent background.
 *
 * IMPORTANT:
 *
 * We intentionally do NOT use setWindowOpacity() here.
 * Instead, opacity is applied directly to the background.
 *
 * This keeps text, icons, buttons and the header crisp.
 */
void PopupWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.setRenderHint(
        QPainter::Antialiasing,
        true
    );

    /*
     * ---------------------------------------------------------
     * Create rounded popup shape
     * ---------------------------------------------------------
     */

    QPainterPath backgroundPath;

    backgroundPath.addRoundedRect(
        rect(),
        14,
        14
    );

    /*
     * Only paint inside the rounded popup.
     */
    painter.setClipPath(backgroundPath);


    /*
     * ---------------------------------------------------------
     * Determine active theme
     * ---------------------------------------------------------
     */

    QString theme = m_settings.theme();

    /*
     * "System" follows the current Qt application palette.
     */
    if (theme == "System")
    {
        const QColor windowColor =
            QApplication::palette().color(
                QPalette::Window
            );

        const int brightness =
            windowColor.red()
            + windowColor.green()
            + windowColor.blue();

        if (brightness < 384)
        {
            theme = "Dark";
        }
        else
        {
            theme = "Light";
        }
    }


    /*
     * ---------------------------------------------------------
     * Get configured opacity
     * ---------------------------------------------------------
     *
     * Example:
     *
     *     0.88 = 88% opaque
     */

    const double configuredOpacity =
        m_settings.popupOpacity();


    /*
     * Convert opacity into Qt's 0-255 alpha range.
     */
    const int alpha =
        qBound(
            0,
            static_cast<int>(
                configuredOpacity * 255.0
            ),
            255
        );


    /*
     * ---------------------------------------------------------
     * Select popup background
     * ---------------------------------------------------------
     */

    QColor backgroundColor;

    if (theme == "Light")
    {
        /*
         * Soft white background.
         *
         * The slight blue/grey tint prevents the popup from
         * looking like a completely flat white rectangle.
         */
        backgroundColor = QColor(
            248,
            250,
            252,
            alpha
        );
    }
    else
    {
        /*
         * Soft charcoal background.
         *
         * Avoid pure black because it looks harsh and makes
         * the interface feel heavier.
         */
        backgroundColor = QColor(
            27,
            30,
            35,
            alpha
        );
    }


    /*
     * ---------------------------------------------------------
     * Paint popup background
     * ---------------------------------------------------------
     */

    painter.fillPath(
        backgroundPath,
        backgroundColor
    );


    /*
     * ---------------------------------------------------------
     * Subtle popup border
     * ---------------------------------------------------------
     */

    if (theme == "Light")
    {
        painter.setPen(
            QColor(
                20,
                30,
                45,
                28
            )
        );
    }
    else
    {
        painter.setPen(
            QColor(
                255,
                255,
                255,
                28
            )
        );
    }

    painter.drawPath(
        backgroundPath
    );
}


/**
 * @brief Applies Clipo visual styling.
 */
/**
 * @brief Applies the currently selected Clipo theme.
 *
 * The actual widget styling is stored in external QSS files:
 *
 *     :/styles/dark.qss
 *     :/styles/light.qss
 *
 * The selected theme comes from SettingsManager.
 *
 * "System" currently follows the Qt application's palette.
 * We will improve automatic OS theme detection later if needed.
 */
void PopupWindow::setupStyle()
{
    QString theme = m_settings.theme();

    /*
     * ---------------------------------------------------------
     * Resolve "System" theme
     * ---------------------------------------------------------
     *
     * Qt provides the application's current palette.
     *
     * If the window background is dark, use the dark theme.
     * Otherwise use the light theme.
     */
    if (theme == "System")
    {
        const QColor windowColor =
            QApplication::palette().color(QPalette::Window);

        const int brightness =
            windowColor.red()
            + windowColor.green()
            + windowColor.blue();

        if (brightness < 384)
        {
            theme = "Dark";
        }
        else
        {
            theme = "Light";
        }
    }


    /*
     * ---------------------------------------------------------
     * Select the QSS resource
     * ---------------------------------------------------------
     */
    QString stylesheetPath;

    if (theme == "Light")
    {
        stylesheetPath = ":/styles/light.qss";
    }
    else
    {
        /*
         * Default to Dark if an unknown value is stored.
         */
        stylesheetPath = ":/styles/dark.qss";
    }


    /*
     * ---------------------------------------------------------
     * Load stylesheet
     * ---------------------------------------------------------
     */
    QFile stylesheetFile(stylesheetPath);

    if (!stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning()
            << "Clipo:"
            << "Could not load stylesheet:"
            << stylesheetPath;

        return;
    }


    const QString stylesheet =
        QString::fromUtf8(
            stylesheetFile.readAll()
        );

    stylesheetFile.close();


    /*
     * Apply the stylesheet to the popup.
     */
    setStyleSheet(stylesheet);


    /*
    * ---------------------------------------------------------
    * Apply popup opacity
    * ---------------------------------------------------------
    */

    // const double opacity =
    //     m_settings.popupOpacity();

    // setWindowOpacity(opacity);


    /*
     * Force the popup to repaint after the theme changes.
     */
    update();
}


/**
 * @brief Creates the popup widgets and layouts.
 */
void PopupWindow::setupUi()
{
    /*
     * Frameless utility popup that stays above other windows.
     */
    setWindowFlags(
        Qt::FramelessWindowHint |
        Qt::Tool |
        Qt::WindowStaysOnTopHint
    );

    /*
     * Fixed popup size for the current version.
     */
    setFixedSize(
        560,
        360
    );


    /*
     * ---------------------------------------------------------
     * Header
     * ---------------------------------------------------------
     */

    m_header =
        new QWidget(
            this
        );

    m_header->setObjectName(
        "PopupHeader"
    );

    m_header->setFixedHeight(
        38
    );

    /*
     * Allow the header to participate in window dragging.
     */
    m_header->installEventFilter(
        this
    );


    /*
     * ---------------------------------------------------------
     * Title
     * ---------------------------------------------------------
     */

    auto *titleLabel =
        new QLabel(
            "Clipo",
            m_header
        );

    titleLabel->setObjectName(
        "PopupTitle"
    );

    /*
     * Allow dragging even when the mouse is over the title.
     */
    // m_titleLabel->installEventFilter(
    //     this
    // );


    /*
     * ---------------------------------------------------------
     * Settings
     * ---------------------------------------------------------
     */

    m_settingsButton =
        new QPushButton(
            "⚙",
            m_header
        );

    m_settingsButton->setObjectName(
        "SettingsButton"
    );

    m_settingsButton->setFixedSize(
        30,
        30
    );

    m_settingsButton->setToolTip(
        "Settings"
    );

    /*
    * Open the Settings dialog.
    */
    connect(
        m_settingsButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            SettingsDialog dialog(
                m_settings,
                this
            );

            /*
            * Forward the shortcut change from the settings dialog.
            *
            * SettingsDialog emits this signal when the user saves
            * the new global shortcut.
            */
            connect(
                &dialog,
                &SettingsDialog::globalShortcutChanged,
                this,
                &PopupWindow::globalShortcutChanged
            );

            if (dialog.exec() == QDialog::Accepted)
            {
                /*
                * Re-apply the popup appearance settings after
                * the settings dialog is closed.
                */
                setupStyle();
            }
        }
    );


    /*
     * ---------------------------------------------------------
     * Minimize
     * ---------------------------------------------------------
     */

    m_minimizeButton =
        new QPushButton(
            "−",
            m_header
        );

    m_minimizeButton->setObjectName(
        "MinimizeButton"
    );

    m_minimizeButton->setFixedSize(
        30,
        30
    );

    m_minimizeButton->setToolTip(
        "Minimize"
    );

    /*
     * For now, minimize simply hides the popup.
     * The application itself continues running in the background.
     */
    connect(
        m_minimizeButton,
        &QPushButton::clicked,
        this,
        &QWidget::hide
    );


    /*
     * ---------------------------------------------------------
     * Close
     * ---------------------------------------------------------
     */

    m_closeButton =
        new QPushButton(
            "×",
            m_header
        );

    m_closeButton->setObjectName(
        "CloseButton"
    );

    m_closeButton->setFixedSize(
        30,
        30
    );

    m_closeButton->setToolTip(
        "Close"
    );

    /*
     * Closing the popup currently hides it.
     * Clipo itself continues running.
     */
    connect(
        m_closeButton,
        &QPushButton::clicked,
        this,
        &QWidget::hide
    );


    /*
     * ---------------------------------------------------------
     * Header layout
     * ---------------------------------------------------------
     */

    auto *headerLayout =
        new QHBoxLayout(
            m_header
        );

    headerLayout->setContentsMargins(
        4,
        2,
        4,
        2
    );

    headerLayout->setSpacing(
        4
    );

    headerLayout->addWidget(
        titleLabel
    );

    headerLayout->addStretch();

    headerLayout->addWidget(
        m_settingsButton
    );

    headerLayout->addWidget(
        m_minimizeButton
    );

    headerLayout->addWidget(
        m_closeButton
    );


    /*
     * ---------------------------------------------------------
     * Search
     * ---------------------------------------------------------
     */

    m_searchBox =
        new QLineEdit(
            this
        );

    m_searchBox->setPlaceholderText(
        "Search clipboard..."
    );

    m_searchBox->setClearButtonEnabled(
        true
    );

    /*
     * The search box receives keyboard input.
     *
     * The event filter allows us to intercept:
     *   Up
     *   Down
     *   Enter
     *   Escape
     */
    m_searchBox->installEventFilter(
        this
    );


    /*
     * ---------------------------------------------------------
     * Clipboard history
     * ---------------------------------------------------------
     */

    m_historyView =
        new ClipboardHistoryView(
            m_database,
            this
        );

    /*
     * IMPORTANT:
     *
     * Install the PopupWindow event filter on the history list
     * as well. Otherwise, when the history list itself has focus,
     * Enter would not reach PopupWindow::eventFilter().
     */
    m_historyView->installEventFilter(
        this
    );

    /*
    * ---------------------------------------------------------
    * Double-click paste
    * ---------------------------------------------------------
    *
    * Double-clicking a clipboard item requests the same paste
    * operation that is normally triggered by pressing Enter.
    */
    connect(
        m_historyView,
        &ClipboardHistoryView::itemDoubleClicked,
        this,
        &PopupWindow::pasteSelectedItem
    );


    /*
     * ---------------------------------------------------------
     * Search handling
     * ---------------------------------------------------------
     */

    connect(
        m_searchBox,
        &QLineEdit::textChanged,
        this,
        [this](const QString &text)
        {
            m_historyView->filter(
                text
            );
        }
    );


    /*
     * ---------------------------------------------------------
     * Main layout
     * ---------------------------------------------------------
     */

    auto *layout =
        new QVBoxLayout(
            this
        );

    layout->setContentsMargins(
        14,
        8,
        14,
        14
    );

    layout->setSpacing(
        6
    );

    layout->addWidget(
        m_header
    );

    layout->addWidget(
        m_searchBox
    );

    layout->addWidget(
        m_historyView
    );

    /*
     * Keep the search box focused when the popup is opened.
     *
     * Enter is handled by eventFilter(), so the user can
     * immediately select/paste using the keyboard.
     */
    m_searchBox->setFocus();
}


/**
 * @brief Refreshes clipboard history.
 */
void PopupWindow::refreshHistory()
{
    m_historyView->refresh();
}


/**
 * @brief Finds the screen containing the mouse cursor.
 *
 * This allows Clipo to appear on the monitor where the
 * user is currently working.
 */
QScreen *PopupWindow::activeScreen() const
{
    const QPoint cursorPosition =
        QCursor::pos();

    QScreen *screen =
        QGuiApplication::screenAt(
            cursorPosition
        );

    /*
     * Fallback for systems where screenAt() returns nullptr.
     */
    if (screen == nullptr)
    {
        screen =
            QGuiApplication::primaryScreen();
    }

    return screen;
}


/**
 * @brief Calculates a smart popup position near the mouse cursor.
 *
 * The popup initially appears to the right and below the cursor.
 * If there is insufficient space, it moves to the opposite side.
 */
QPoint PopupWindow::calculatePopupPosition(
    QScreen *screen
) const
{
    if (screen == nullptr)
    {
        return QPoint(
            0,
            0
        );
    }

    const QRect availableArea =
        screen->availableGeometry();

    const QPoint cursorPosition =
        QCursor::pos();

    constexpr int margin = 20;
    constexpr int cursorGap = 16;

    /*
     * Initially place popup to the right and below the cursor.
     */
    int x =
        cursorPosition.x()
        + cursorGap;

    int y =
        cursorPosition.y()
        + cursorGap;


    /*
     * If it does not fit on the right,
     * move it to the left.
     */
    if (
        x + width()
        >
        availableArea.right()
    )
    {
        x =
            cursorPosition.x()
            - width()
            - cursorGap;
    }


    /*
     * If it does not fit below,
     * move it above the cursor.
     */
    if (
        y + height()
        >
        availableArea.bottom()
    )
    {
        y =
            cursorPosition.y()
            - height()
            - cursorGap;
    }


    /*
     * Final monitor-boundary protection.
     */
    x = qBound(
        availableArea.left() + margin,
        x,
        availableArea.right()
            - width()
            - margin
    );

    y = qBound(
        availableArea.top() + margin,
        y,
        availableArea.bottom()
            - height()
            - margin
    );


    return QPoint(
        x,
        y
    );
}


/**
 * @brief Shows Clipo.
 *
 * IMPORTANT:
 *
 * The currently active application is remembered BEFORE
 * Clipo calls show(), raise(), or activateWindow().
 *
 * This allows PasteController to later return to the
 * application from which Clipo was opened.
 */
void PopupWindow::showPopup()
{
    /*
     * Remember the currently active X11 window before
     * Clipo becomes the active window.
     */
    const bool remembered =
        PasteController::rememberActiveWindow();

    qDebug()
        << "Clipo:"
        << "Previous application remembered ="
        << remembered;


    /*
     * Reload clipboard history from SQLite.
     */
    refreshHistory();


    /*
     * Calculate popup position using the current cursor.
     */
    QScreen *screen =
        activeScreen();

    if (screen != nullptr)
    {
        move(
            calculatePopupPosition(
                screen
            )
        );
    }


    /*
     * Now show Clipo.
     */
    show();

    raise();

    activateWindow();


    /*
     * Keep the search box focused.
     *
     * Enter is intercepted by eventFilter().
     */
    m_searchBox->setFocus();


    /*
     * Select the first history item if no item
     * is currently selected.
     */
    if (
        m_historyView->currentItem()
        ==
        nullptr
    )
    {
        m_historyView->setCurrentRow(
            0
        );
    }
}


/**
 * @brief Pastes the currently selected clipboard item.
 *
 * The sequence is:
 *
 * 1. Get selected clipboard text.
 * 2. Hide Clipo.
 * 3. Allow the window manager to process the hide.
 * 4. Restore the previously active application.
 * 5. Send Ctrl+V through PasteController.
 * 6. Forget the previous application.
 */
void PopupWindow::pasteSelectedItem()
{
    qDebug()
        << "Clipo: pasteSelectedItem() called.";


    /*
     * Get the complete clipboard content associated
     * with the selected history item.
     */
    const QString text =
        m_historyView->selectedContent();


    /*
     * Do nothing if there is no valid selected item.
     */
    if (text.isEmpty())
    {
        qDebug()
            << "Clipo:"
            << "No clipboard item selected.";

        return;
    }


    qDebug()
        << "Clipo:"
        << "Pasting selected item.";


    /*
     * Hide Clipo before restoring the previous application.
     */
    hide();

    /*
     * Give Qt/X11 a chance to process the hide operation.
     */
    QApplication::processEvents();


    /*
     * Restore the previous application and perform Ctrl+V.
     */
    const bool success =
        PasteController::paste(
            text
        );


    if (!success)
    {
        qWarning()
            << "Clipo:"
            << "Paste operation failed.";
    }


    /*
     * The stored target window is no longer needed.
     */
    PasteController::clearActiveWindow();
}


/**
 * @brief Handles keyboard events sent directly to PopupWindow.
 *
 * Escape closes the popup.
 *
 * Most keyboard events are handled through eventFilter()
 * because the search box and history list normally have focus.
 */
void PopupWindow::keyPressEvent(
    QKeyEvent *event
)
{
    if (
        event->key()
        ==
        Qt::Key_Escape
    )
    {
        hide();

        return;
    }

    QWidget::keyPressEvent(
        event
    );
}


/**
 * @brief Handles mouse press events.
 */
void PopupWindow::mousePressEvent(
    QMouseEvent *event
)
{
    QWidget::mousePressEvent(
        event
    );
}


/**
 * @brief Handles mouse movement.
 *
 * The header event filter controls the beginning of a drag.
 * This function performs the actual movement.
 */
void PopupWindow::mouseMoveEvent(
    QMouseEvent *event
)
{
    if (
        m_dragging &&
        (event->buttons() & Qt::LeftButton)
    )
    {
        const QPoint newPosition =
            event->globalPosition().toPoint()
            - m_dragOffset;

        move(
            newPosition
        );

        event->accept();

        return;
    }

    QWidget::mouseMoveEvent(
        event
    );
}


/**
 * @brief Handles mouse release.
 */
void PopupWindow::mouseReleaseEvent(
    QMouseEvent *event
)
{
    if (
        event->button()
        ==
        Qt::LeftButton
    )
    {
        m_dragging = false;

        event->accept();

        return;
    }

    QWidget::mouseReleaseEvent(
        event
    );
}


/**
 * @brief Handles keyboard events from the search box/history list
 *        and mouse events from the draggable header.
 */
bool PopupWindow::eventFilter(
    QObject *watched,
    QEvent *event
)
{
    /*
     * ---------------------------------------------------------
     * Keyboard handling
     * ---------------------------------------------------------
     */
    if (
        event->type()
        ==
        QEvent::KeyPress
    )
    {
        auto *keyEvent =
            static_cast<QKeyEvent *>(
                event
            );


        /*
         * -----------------------------------------------------
         * Enter / Return
         *
         * Pressing Enter pastes the selected clipboard item.
         * This works whether the search box OR history list
         * currently has focus.
         * -----------------------------------------------------
         */
        if (
            keyEvent->key()
            ==
            Qt::Key_Return
            ||
            keyEvent->key()
            ==
            Qt::Key_Enter
        )
        {
            qDebug()
                << "Clipo: Enter pressed.";

            pasteSelectedItem();

            return true;
        }


        /*
         * -----------------------------------------------------
         * Escape
         *
         * Hide Clipo without terminating the application.
         * -----------------------------------------------------
         */
        if (
            keyEvent->key()
            ==
            Qt::Key_Escape
        )
        {
            qDebug()
                << "Clipo: Escape pressed.";

            hide();

            return true;
        }


        /*
         * -----------------------------------------------------
         * Up / Down navigation
         *
         * These are intercepted only when the search box
         * has focus.
         *
         * This allows the user to type normally while using
         * Up/Down to move through clipboard history.
         * -----------------------------------------------------
         */
        if (
            watched
            ==
            m_searchBox
        )
        {
            if (
                keyEvent->key()
                ==
                Qt::Key_Down
            )
            {
                m_historyView->selectNext();

                return true;
            }


            if (
                keyEvent->key()
                ==
                Qt::Key_Up
            )
            {
                m_historyView->selectPrevious();

                return true;
            }
        }
    }


    /*
     * ---------------------------------------------------------
     * Header dragging
     * ---------------------------------------------------------
     */

    if (
        event->type()
        ==
        QEvent::MouseButtonPress
    )
    {
        auto *mouseEvent =
            static_cast<QMouseEvent *>(
                event
            );

        /*
         * Only the left mouse button starts dragging.
         */
        if (
            mouseEvent->button()
            ==
            Qt::LeftButton
        )
        {
            /*
             * Calculate the offset between the cursor and
             * the popup's top-left corner.
             */
            m_dragOffset =
                mouseEvent->globalPosition().toPoint()
                - frameGeometry().topLeft();

            m_dragging = true;

            return true;
        }
    }


    /*
     * Stop dragging when the mouse button is released.
     */
    if (
        event->type()
        ==
        QEvent::MouseButtonRelease
    )
    {
        auto *mouseEvent =
            static_cast<QMouseEvent *>(
                event
            );

        if (
            mouseEvent->button()
            ==
            Qt::LeftButton
        )
        {
            m_dragging = false;

            return true;
        }
    }


    /*
     * Move the popup while the header is being dragged.
     */
    if (
        event->type()
        ==
        QEvent::MouseMove
    )
    {
        auto *mouseEvent =
            static_cast<QMouseEvent *>(
                event
            );

        if (
            m_dragging
            &&
            (mouseEvent->buttons() & Qt::LeftButton)
        )
        {
            const QPoint newPosition =
                mouseEvent->globalPosition().toPoint()
                - m_dragOffset;

            move(
                newPosition
            );

            return true;
        }
    }


    /*
     * Let Qt process all events that Clipo does not handle.
     */
    return QWidget::eventFilter(
        watched,
        event
    );
}
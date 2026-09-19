#include "PopupWindow.h"

#include "../database/Database.h"

#include <QAbstractItemView>
#include <QCursor>
#include <QEvent>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMouseEvent>
#include <QPushButton>
#include <QScreen>
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>


/**
 * @brief Creates the Clipo popup.
 */
PopupWindow::PopupWindow(
    Database &database,
    QWidget *parent
)
    : QWidget(parent),
      m_database(database),
      m_header(nullptr),
      m_titleLabel(nullptr),
      m_settingsButton(nullptr),
      m_minimizeButton(nullptr),
      m_closeButton(nullptr),
      m_searchBox(nullptr),
      m_historyList(nullptr),
      m_dragOffset(),
      m_dragging(false)
{
    /*
     * Give the popup a unique object name.
     *
     * This allows the stylesheet to specifically target
     * the top-level popup using:
     *
     *     QWidget#PopupWindow
     */
    setObjectName("PopupWindow");


    /*
     * The popup uses a custom rounded background.
     *
     * WA_TranslucentBackground allows the area outside the
     * rounded popup to remain transparent.
     */
    setAttribute(
        Qt::WA_TranslucentBackground
    );


    /*
     * Allows Qt stylesheets to paint the background of
     * the translucent top-level widget.
     */
    setAttribute(
        Qt::WA_StyledBackground,
        true
    );


    /*
     * Create the UI.
     */
    setupUi();


    /*
     * Apply visual styling.
     */
    setupStyle();
}


/**
 * @brief Applies the visual style to the Clipo popup.
 *
 * The popup uses a mostly opaque dark glass appearance.
 * The background is approximately 88% opaque, allowing a small
 * amount of the desktop wallpaper to remain visible.
 * 
 */





 /**
 * @brief Paints the main Clipo popup background.
 *
 * We intentionally paint the main popup ourselves instead of
 * depending on the stylesheet to render the background.
 *
 * This is more reliable for a frameless window using
 * WA_TranslucentBackground.
 *
 * Alpha:
 *
 *     225 / 255 ≈ 88% opaque
 *
 * This gives us the desired semi-transparent appearance:
 *
 *     Desktop wallpaper
 *            ↓
 *     ┌───────────────────────┐
 *     │       Clipo            │
 *     │                        │
 *     │       History          │
 *     └───────────────────────┘
 *
 * The wallpaper is only subtly visible instead of being
 * directly visible through the entire popup.
 */
void PopupWindow::paintEvent(
    QPaintEvent *event
)
{
    Q_UNUSED(event);

    /*
     * Create a painter for this popup.
     */
    QPainter painter(this);

    /*
     * Enable smooth rendering for the rounded corners.
     */
    painter.setRenderHint(
        QPainter::Antialiasing,
        true
    );


    /*
     * Create a rounded rectangle covering the entire popup.
     */
    QPainterPath backgroundPath;

    backgroundPath.addRoundedRect(
        rect(),
        14,
        14
    );


    /*
     * Clip painting to the rounded rectangle.
     */
    painter.setClipPath(
        backgroundPath
    );


    /*
     * ---------------------------------------------------------
     * Main popup background
     * ---------------------------------------------------------
     *
     * 225 / 255 ≈ 88% opacity.
     *
     * This is intentionally NOT fully opaque because we still
     * want a subtle desktop-glass effect.
     */
    painter.fillPath(
        backgroundPath,
        QColor(
            30,
            32,
            36,
            225
        )
    );


    /*
     * ---------------------------------------------------------
     * Subtle top highlight
     * ---------------------------------------------------------
     *
     * Adds a very subtle glass-like highlight to the popup.
     */
    painter.setPen(
        QColor(
            255,
            255,
            255,
            35
        )
    );

    painter.drawPath(
        backgroundPath
    );
}


void PopupWindow::setupStyle()
{
    setStyleSheet(R"(

        /*
         * =====================================================
         * Main popup
         * =====================================================
         *
         * The actual background is painted by paintEvent().
         *
         * We keep the stylesheet background transparent so it
         * does not interfere with the custom QPainter background.
         */
        QWidget#PopupWindow {

            background:
                transparent;

            border:
                none;
        }


        /*
         * =====================================================
         * Header
         * =====================================================
         */
        QWidget#PopupHeader {

            /*
             * The header intentionally has its own opaque-ish
             * background.
             */
            background:
                rgba(30, 32, 36, 245);

            border:
                none;

            border-radius:
                10px;
        }


        /*
         * =====================================================
         * Title
         * =====================================================
         */
        QLabel#PopupTitle {

            background:
                transparent;

            color:
                rgba(255, 255, 255, 225);

            font-size:
                14px;

            font-weight:
                600;

            padding-left:
                6px;
        }


        /*
         * =====================================================
         * Settings
         * =====================================================
         */
        QPushButton#SettingsButton {

            background:
                transparent;

            border:
                none;

            border-radius:
                7px;

            color:
                rgba(255, 255, 255, 190);

            font-size:
                16px;
        }

        QPushButton#SettingsButton:hover {

            background:
                rgba(255, 255, 255, 30);

            color:
                #ffffff;
        }


        /*
         * =====================================================
         * Minimize
         * =====================================================
         */
        QPushButton#MinimizeButton {

            background:
                transparent;

            border:
                none;

            border-radius:
                7px;

            color:
                rgba(255, 255, 255, 190);

            font-size:
                18px;
        }

        QPushButton#MinimizeButton:hover {

            background:
                rgba(255, 255, 255, 30);

            color:
                #ffffff;
        }


        /*
         * =====================================================
         * Close
         * =====================================================
         */
        QPushButton#CloseButton {

            background:
                transparent;

            border:
                none;

            border-radius:
                7px;

            color:
                rgba(255, 255, 255, 190);

            font-size:
                18px;
        }

        QPushButton#CloseButton:hover {

            background:
                rgba(220, 70, 70, 150);

            color:
                #ffffff;
        }


        /*
         * =====================================================
         * Search
         * =====================================================
         */
        QLineEdit {

            background:
                rgba(255, 255, 255, 30);

            border:
                1px solid rgba(255, 255, 255, 75);

            border-radius:
                10px;

            padding:
                10px 12px;

            color:
                #ffffff;

            selection-background-color:
                rgba(80, 150, 255, 100);

            font-size:
                15px;
        }

        QLineEdit:focus {

            border:
                1px solid rgba(255, 255, 255, 130);
        }

        QLineEdit::placeholder {

            color:
                rgba(255, 255, 255, 165);
        }


        /*
         * =====================================================
         * Clipboard list
         * =====================================================
         */
        QListWidget {

            background:
                transparent;

            border:
                none;

            outline:
                none;

            color:
                #ffffff;

            font-size:
                14px;
        }


        /*
         * Individual clipboard item.
         */
        QListWidget::item {

            padding:
                9px 12px;

            border-radius:
                8px;
        }


        /*
         * Selected clipboard item.
         */
        QListWidget::item:selected {

            background:
                rgba(70, 140, 210, 105);

            color:
                #ffffff;
        }


        /*
         * =====================================================
         * Vertical scrollbar
         * =====================================================
         */
        QScrollBar:vertical {

            width:
                6px;

            background:
                transparent;
        }

        QScrollBar::handle:vertical {

            background:
                rgba(255, 255, 255, 85);

            border-radius:
                3px;
        }

        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {

            height:
                0px;
        }


        /*
         * =====================================================
         * Horizontal scrollbar
         * =====================================================
         *
         * Completely disabled visually.
         */
        QScrollBar:horizontal {

            height:
                0px;

            background:
                transparent;
        }

        QScrollBar::handle:horizontal {

            height:
                0px;

            background:
                transparent;
        }

        QScrollBar::add-line:horizontal,
        QScrollBar::sub-line:horizontal {

            width:
                0px;

            height:
                0px;
        }

    )");
}


/**
 * @brief Creates all popup widgets and layouts.
 */
void PopupWindow::setupUi()
{
    /*
     * =========================================================
     * Window configuration
     * =========================================================
     *
     * Clipo is a utility popup instead of a normal application
     * window.
     *
     * FramelessWindowHint:
     *     Removes the normal operating-system title bar.
     *
     * Tool:
     *     Makes this a utility-style window.
     *
     * WindowStaysOnTopHint:
     *     Keeps Clipo above the application currently in use.
     */
    setWindowFlags(
        Qt::FramelessWindowHint |
        Qt::Tool |
        Qt::WindowStaysOnTopHint
    );


    /*
     * Initial popup size.
     *
     * We can make the size dynamic later.
     */
    setFixedSize(
        560,
        360
    );


    /*
     * =========================================================
     * Header
     * =========================================================
     *
     * The header contains:
     *
     *     Clipo                 ⚙   −   ×
     *
     * The header also acts as the draggable area.
     */
    m_header = new QWidget(this);

    m_header->setObjectName(
        "PopupHeader"
    );

    m_header->setFixedHeight(
        38
    );


    /*
     * Install an event filter so that mouse events occurring
     * on the header can be used to drag the popup.
     */
    m_header->installEventFilter(this);


    /*
     * ---------------------------------------------------------
     * Title
     * ---------------------------------------------------------
     */
    m_titleLabel = new QLabel(
        "Clipo",
        m_header
    );

    m_titleLabel->setObjectName(
        "PopupTitle"
    );


    /*
     * The title is part of the draggable header.
     */
    m_titleLabel->installEventFilter(this);


    /*
     * =========================================================
     * Settings button
     * =========================================================
     *
     * The settings window itself will be implemented later.
     *
     * Planned settings include:
     *
     *  - System / Light / Dark theme
     *  - Transparency
     *  - Popup position
     *  - Popup size
     *  - Global shortcut
     *  - Maximum history size
     *  - Startup behaviour
     *  - Privacy/storage options
     */
    m_settingsButton = new QPushButton(
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
     * The settings functionality will be connected later.
     *
     * For now the button is only part of the UI.
     */


    /*
     * =========================================================
     * Minimize button
     * =========================================================
     *
     * Since Clipo is a popup utility, minimizing currently
     * means hiding the popup.
     *
     * The application itself remains running so that the
     * ClipboardManager can continue monitoring the system
     * clipboard.
     */
    m_minimizeButton = new QPushButton(
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


    connect(
        m_minimizeButton,
        &QPushButton::clicked,
        this,
        &QWidget::hide
    );


    /*
     * =========================================================
     * Close button
     * =========================================================
     *
     * Close also hides the popup for now.
     *
     * We don't terminate Clipo because the clipboard monitor
     * should continue running in the background.
     */
    m_closeButton = new QPushButton(
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


    connect(
        m_closeButton,
        &QPushButton::clicked,
        this,
        &QWidget::hide
    );


    /*
     * =========================================================
     * Header layout
     * =========================================================
     */
    auto *headerLayout =
        new QHBoxLayout(m_header);

    headerLayout->setContentsMargins(
        4,
        2,
        4,
        2
    );

    headerLayout->setSpacing(
        4
    );


    /*
     * Title on the left.
     */
    headerLayout->addWidget(
        m_titleLabel
    );


    /*
     * Push the controls to the right.
     */
    headerLayout->addStretch();


    /*
     * Settings
     */
    headerLayout->addWidget(
        m_settingsButton
    );


    /*
     * Minimize
     */
    headerLayout->addWidget(
        m_minimizeButton
    );


    /*
     * Close
     */
    headerLayout->addWidget(
        m_closeButton
    );


    /*
     * =========================================================
     * Search box
     * =========================================================
     */
    m_searchBox = new QLineEdit(
        this
    );

    m_searchBox->setPlaceholderText(
        "Search clipboard..."
    );

    /*
     * Show a clear button when search text exists.
     */
    m_searchBox->setClearButtonEnabled(
        true
    );


    /*
     * =========================================================
     * Clipboard history
     * =========================================================
     */
    m_historyList = new QListWidget(
        this
    );


    /*
     * Only one clipboard entry can be selected at a time.
     */
    m_historyList->setSelectionMode(
        QAbstractItemView::SingleSelection
    );


    /*
     * Never show a horizontal scrollbar.
     *
     * Long clipboard content will be shortened or wrapped
     * instead.
     */
    m_historyList->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );


    /*
     * Only show a vertical scrollbar when required.
     */
    m_historyList->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );


    /*
     * Allow the selected item to wrap its text when expanded.
     *
     * Normal items contain a single-line preview, so they
     * remain compact.
     */
    m_historyList->setWordWrap(
        true
    );


    /*
     * Don't force horizontal text scrolling.
     */
    m_historyList->setTextElideMode(
        Qt::ElideRight
    );


    /*
     * When the user clicks an item, expand only that item.
     *
     * Keyboard navigation will not expand items automatically.
     * This keeps ↑/↓ navigation compact.
     */
    connect(
        m_historyList,
        &QListWidget::itemClicked,
        this,
        &PopupWindow::expandHistoryItem
    );


    /*
     * =========================================================
     * Main layout
     * =========================================================
     */
    auto *layout =
        new QVBoxLayout(this);

    layout->setContentsMargins(
        14,
        8,
        14,
        14
    );

    layout->setSpacing(
        6
    );


    /*
     * Header
     */
    layout->addWidget(
        m_header
    );


    /*
     * Search
     */
    layout->addWidget(
        m_searchBox
    );


    /*
     * History
     */
    layout->addWidget(
        m_historyList
    );


    /*
     * =========================================================
     * Load clipboard history
     * =========================================================
     */
    loadHistory();


    /*
     * Search box is ready for typing.
     */
    m_searchBox->setFocus();


    /*
     * Select the newest entry.
     */
    if (m_historyList->count() > 0) {

        m_historyList->setCurrentRow(
            0
        );
    }
}


/**
 * @brief Creates a compact preview of clipboard content.
 */
QString PopupWindow::createPreview(
    const QString &content
) const
{
    /*
     * Start with the original clipboard content.
     */
    QString preview = content;


    /*
     * Convert newlines into a visible separator.
     *
     * This keeps the normal history item to one visual line.
     */
    preview.replace(
        '\n',
        " ↵ "
    );

    preview.replace(
        '\r',
        ""
    );


    /*
     * Collapse unnecessary whitespace.
     */
    preview = preview.simplified();


    /*
     * Keep the popup compact.
     */
    constexpr int maxPreviewLength = 100;


    if (preview.length() > maxPreviewLength) {

        preview =
            preview.left(maxPreviewLength)
            + "...";
    }


    return preview;
}


/**
 * @brief Loads clipboard history from SQLite.
 */
void PopupWindow::loadHistory()
{
    /*
     * Remove existing entries before loading fresh data.
     */
    m_historyList->clear();


    /*
     * Ask Database for the newest 50 entries.
     */
    const std::vector<ClipboardEntry> history =
        m_database.getHistory(50);


    /*
     * ---------------------------------------------------------
     * Empty database
     * ---------------------------------------------------------
     */
    if (history.empty()) {

        auto *item =
            new QListWidgetItem(
                "No clipboard history yet."
            );


        /*
         * The empty-state message should not be selectable.
         */
        item->setFlags(
            Qt::NoItemFlags
        );


        m_historyList->addItem(
            item
        );

        return;
    }


    /*
     * ---------------------------------------------------------
     * Add history entries
     * ---------------------------------------------------------
     */
    for (const ClipboardEntry &entry : history) {

        /*
         * Create a short display preview.
         */
        const QString preview =
            createPreview(
                entry.content
            );


        /*
         * Create the visible list item.
         */
        auto *item =
            new QListWidgetItem(
                preview
            );


        /*
         * Give normal entries a compact height.
         */
        item->setSizeHint(
            QSize(
                -1,
                40
            )
        );


        /*
         * Store the database ID.
         *
         * This will later allow operations such as:
         *
         *  - Delete
         *  - Pin
         *  - Update
         */
        item->setData(
            Qt::UserRole,
            entry.id
        );


        /*
         * Store the complete original clipboard content.
         *
         * The visible text is only a preview, but the complete
         * content remains available for the future paste operation.
         */
        item->setData(
            Qt::UserRole + 1,
            entry.content
        );


        /*
         * Add the item to the list.
         */
        m_historyList->addItem(
            item
        );
    }


    /*
     * Select the newest entry.
     */
    m_historyList->setCurrentRow(
        0
    );
}


/**
 * @brief Refreshes the visible clipboard history.
 */
void PopupWindow::refreshHistory()
{
    /*
     * Reload the latest data from SQLite.
     *
     * This is called after ClipboardManager detects a new
     * clipboard entry.
     */
    loadHistory();
}


/**
 * @brief Expands the clicked clipboard item.
 *
 * Only the clicked item is expanded.
 * All other entries remain compact.
 */
void PopupWindow::expandHistoryItem(
    QListWidgetItem *item
)
{
    /*
     * Safety check.
     */
    if (item == nullptr) {
        return;
    }


    /*
     * ---------------------------------------------------------
     * Collapse all other items
     * ---------------------------------------------------------
     */
    for (int index = 0;
         index < m_historyList->count();
         ++index) {

        QListWidgetItem *currentItem =
            m_historyList->item(index);


        if (currentItem == item) {
            continue;
        }


        /*
         * Retrieve the original clipboard content.
         */
        const QString fullText =
            currentItem->data(
                Qt::UserRole + 1
            ).toString();


        /*
         * Restore the compact preview.
         */
        currentItem->setText(
            createPreview(
                fullText
            )
        );


        /*
         * Restore compact height.
         */
        currentItem->setSizeHint(
            QSize(
                -1,
                40
            )
        );
    }


    /*
     * ---------------------------------------------------------
     * Expand the clicked item
     * ---------------------------------------------------------
     */
    const QString fullText =
        item->data(
            Qt::UserRole + 1
        ).toString();


    /*
     * Display the complete clipboard content.
     *
     * QListWidget word wrapping prevents the text from creating
     * a horizontal scrollbar.
     */
    item->setText(
        fullText
    );


    /*
     * Give the selected item more vertical space.
     *
     * 80 pixels is a starting value.
     * We can later make this dynamic based on content length.
     */
    item->setSizeHint(
        QSize(
            -1,
            80
        )
    );
}


/**
 * @brief Finds the screen containing the mouse cursor.
 */
QScreen *PopupWindow::activeScreen() const
{
    /*
     * Get the current global cursor position.
     */
    const QPoint cursorPosition =
        QCursor::pos();


    /*
     * Find the monitor containing the cursor.
     *
     * This supports multi-monitor setups.
     */
    QScreen *screen =
        QGuiApplication::screenAt(
            cursorPosition
        );


    /*
     * Fall back to the primary screen if necessary.
     */
    if (screen == nullptr) {

        screen =
            QGuiApplication::primaryScreen();
    }


    return screen;
}


/**
 * @brief Calculates a safe popup position near the cursor.
 */
QPoint PopupWindow::calculatePopupPosition(
    QScreen *screen
) const
{
    /*
     * Safety check.
     */
    if (screen == nullptr) {

        return QPoint(
            0,
            0
        );
    }


    /*
     * availableGeometry() represents the usable portion
     * of the monitor, normally excluding the taskbar/panel.
     */
    const QRect availableArea =
        screen->availableGeometry();


    /*
     * Current cursor position.
     */
    const QPoint cursorPosition =
        QCursor::pos();


    /*
     * Keep a small distance from the monitor edges.
     */
    constexpr int margin = 20;


    /*
     * Distance between the cursor and popup.
     */
    constexpr int cursorGap = 16;


    /*
     * First attempt:
     *
     *     below + right of cursor
     */
    int x =
        cursorPosition.x()
        + cursorGap;

    int y =
        cursorPosition.y()
        + cursorGap;


    /*
     * If there is not enough room on the right,
     * place the popup on the left side of the cursor.
     */
    if (
        x + width()
        > availableArea.right()
    ) {

        x =
            cursorPosition.x()
            - width()
            - cursorGap;
    }


    /*
     * If there is not enough room below,
     * place the popup above the cursor.
     */
    if (
        y + height()
        > availableArea.bottom()
    ) {

        y =
            cursorPosition.y()
            - height()
            - cursorGap;
    }


    /*
     * Final safety check.
     *
     * This guarantees that the complete popup remains visible.
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
 * @brief Shows the popup at a smart location.
 */
void PopupWindow::showPopup()
{
    /*
     * Find the monitor containing the cursor.
     */
    QScreen *screen =
        activeScreen();


    /*
     * Calculate the popup's position.
     */
    move(
        calculatePopupPosition(
            screen
        )
    );


    /*
     * Make the popup visible.
     */
    show();


    /*
     * Bring it above other windows.
     */
    raise();


    /*
     * Activate the popup.
     */
    activateWindow();


    /*
     * Focus the search box.
     */
    m_searchBox->setFocus();


    /*
     * Select the newest clipboard entry.
     */
    if (m_historyList->count() > 0) {

        m_historyList->setCurrentRow(
            0
        );
    }
}


/**
 * @brief Handles keyboard input.
 */
void PopupWindow::keyPressEvent(
    QKeyEvent *event
)
{
    /*
     * Escape hides the popup.
     *
     * It does not terminate the Clipo process.
     */
    if (
        event->key()
        == Qt::Key_Escape
    ) {

        hide();

        return;
    }


    /*
     * Pass all other keys to QWidget.
     */
    QWidget::keyPressEvent(
        event
    );
}


/**
 * @brief Handles mouse press events.
 *
 * The popup itself is frameless, so normal OS window dragging
 * is unavailable.
 *
 * Dragging is handled specifically for the header.
 */
void PopupWindow::mousePressEvent(
    QMouseEvent *event
)
{
    /*
     * This function mainly handles events that reach the
     * PopupWindow itself.
     *
     * Header events are handled through eventFilter().
     */
    QWidget::mousePressEvent(
        event
    );
}


/**
 * @brief Handles mouse movement for popup dragging.
 */
void PopupWindow::mouseMoveEvent(
    QMouseEvent *event
)
{
    /*
     * If dragging was started through the header,
     * move the popup with the cursor.
     */
    if (
        m_dragging &&
        (event->buttons() & Qt::LeftButton)
    ) {

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
 * @brief Handles mouse release events.
 */
void PopupWindow::mouseReleaseEvent(
    QMouseEvent *event
)
{
    /*
     * Release the drag state when the left mouse button
     * is released.
     */
    if (
        event->button()
        == Qt::LeftButton
    ) {

        m_dragging = false;

        event->accept();

        return;
    }


    QWidget::mouseReleaseEvent(
        event
    );
}


/**
 * @brief Handles mouse events from the header.
 *
 * Since the header is a child widget, its mouse events normally
 * do not reach PopupWindow's mousePressEvent().
 *
 * The event filter lets us implement dragging correctly without
 * affecting the search box or clipboard list.
 */
bool PopupWindow::eventFilter(
    QObject *watched,
    QEvent *event
)
{
    /*
     * Only the header and title are draggable.
     */
    const bool isDragArea =
        watched == m_header ||
        watched == m_titleLabel;


    if (!isDragArea) {

        return QWidget::eventFilter(
            watched,
            event
        );
    }


    /*
     * ---------------------------------------------------------
     * Mouse press
     * ---------------------------------------------------------
     */
    if (
        event->type()
        == QEvent::MouseButtonPress
    ) {

        auto *mouseEvent =
            static_cast<QMouseEvent *>(event);


        /*
         * Only the left mouse button starts dragging.
         */
        if (
            mouseEvent->button()
            == Qt::LeftButton
        ) {

            /*
             * Store the distance between the cursor and the
             * popup's top-left corner.
             */
            m_dragOffset =
                mouseEvent
                    ->globalPosition()
                    .toPoint()
                - frameGeometry().topLeft();


            m_dragging = true;


            mouseEvent->accept();

            return true;
        }
    }


    /*
     * ---------------------------------------------------------
     * Mouse movement
     * ---------------------------------------------------------
     */
    if (
        event->type()
        == QEvent::MouseMove
    ) {

        auto *mouseEvent =
            static_cast<QMouseEvent *>(event);


        if (
            m_dragging &&
            (mouseEvent->buttons()
             & Qt::LeftButton)
        ) {

            const QPoint newPosition =
                mouseEvent
                    ->globalPosition()
                    .toPoint()
                - m_dragOffset;


            move(
                newPosition
            );


            mouseEvent->accept();

            return true;
        }
    }


    /*
     * ---------------------------------------------------------
     * Mouse release
     * ---------------------------------------------------------
     */
    if (
        event->type()
        == QEvent::MouseButtonRelease
    ) {

        auto *mouseEvent =
            static_cast<QMouseEvent *>(event);


        if (
            mouseEvent->button()
            == Qt::LeftButton
        ) {

            m_dragging = false;

            mouseEvent->accept();

            return true;
        }
    }


    return QWidget::eventFilter(
        watched,
        event
    );
}
#ifndef CLIPO_POPUP_WINDOW_H
#define CLIPO_POPUP_WINDOW_H

#include <QPoint>
#include <QString>
#include <QWidget>

class Database;
class ClipboardHistoryView;
class SettingsManager;

class QLabel;
class QLineEdit;
class QPushButton;
class QScreen;

class QEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;


/**
 * @brief Main Clipo popup window.
 *
 * Responsible for the popup UI and interaction.
 *
 * Clipboard history/search logic is handled by
 * ClipboardHistoryView.
 *
 * Platform-specific paste logic is handled by
 * PasteController.
 *
 * User preferences are handled by SettingsManager.
 */
class PopupWindow : public QWidget
{
    Q_OBJECT

public:

    /**
     * @brief Creates the main Clipo popup window.
     *
     * SettingsManager is passed by reference so the popup
     * can read and update user preferences.
     */
    explicit PopupWindow(
        Database &database,
        SettingsManager &settings,
        QWidget *parent = nullptr
    );


    /**
     * @brief Shows Clipo near the mouse cursor.
     */
    void showPopup();


    /**
     * @brief Refreshes clipboard history.
     */
    void refreshHistory();


signals:

    /**
     * @brief Emitted when the user changes the global
     * keyboard shortcut.
     *
     * The main application uses this signal to unregister
     * the old shortcut and register the new one.
     */
    void globalShortcutChanged(
        const QString &shortcut
    );


protected:

    void paintEvent(
        QPaintEvent *event
    ) override;


    void keyPressEvent(
        QKeyEvent *event
    ) override;


    void mousePressEvent(
        QMouseEvent *event
    ) override;


    void mouseMoveEvent(
        QMouseEvent *event
    ) override;


    void mouseReleaseEvent(
        QMouseEvent *event
    ) override;


    bool eventFilter(
        QObject *watched,
        QEvent *event
    ) override;


private:

    /**
     * @brief Creates and arranges the popup widgets.
     */
    void setupUi();


    /**
     * @brief Applies the current popup styling.
     */
    void setupStyle();


    /**
     * @brief Returns the screen currently containing the mouse.
     */
    QScreen *activeScreen() const;


    /**
     * @brief Calculates the popup position.
     *
     * The popup is positioned near the mouse cursor while
     * keeping it inside the available screen area.
     */
    QPoint calculatePopupPosition(
        QScreen *screen
    ) const;


    /**
     * @brief Pastes the selected clipboard item.
     */
    void pasteSelectedItem();


private:

    /**
     * @brief Database containing clipboard history.
     */
    Database &m_database;


    /**
     * @brief Application settings.
     *
     * Stores preferences such as theme, opacity,
     * popup position and global shortcut.
     */
    SettingsManager &m_settings;


    /**
     * @brief Popup header widget.
     *
     * The entire header acts as the drag area.
     */
    QWidget *m_header;


    /**
     * @brief Opens the settings dialog.
     */
    QPushButton *m_settingsButton;


    /**
     * @brief Minimizes/hides the popup.
     */
    QPushButton *m_minimizeButton;


    /**
     * @brief Closes/hides the popup.
     */
    QPushButton *m_closeButton;


    /**
     * @brief Clipboard history search field.
     */
    QLineEdit *m_searchBox;


    /**
     * @brief Displays clipboard history entries.
     */
    ClipboardHistoryView *m_historyView;


    /**
     * @brief Offset used while dragging the popup.
     */
    QPoint m_dragOffset;


    /**
     * @brief Whether the popup is currently being dragged.
     */
    bool m_dragging;
};

#endif
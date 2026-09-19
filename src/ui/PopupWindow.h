#ifndef CLIPO_POPUP_WINDOW_H
#define CLIPO_POPUP_WINDOW_H

#include <QPoint>
#include <QWidget>

class Database;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QScreen;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;

/**
 * @brief Floating popup used to browse Clipo clipboard history.
 *
 * PopupWindow is responsible for the visual presentation and
 * interaction of the Clipo clipboard manager.
 *
 * Responsibilities:
 *
 *  - Display clipboard history.
 *  - Provide clipboard search UI.
 *  - Position itself near the cursor.
 *  - Allow the popup to be dragged.
 *  - Provide settings, minimize and close controls.
 *  - Expand selected clipboard entries.
 *  - Handle keyboard actions.
 *
 * Database storage remains the responsibility of Database.
 */
class PopupWindow : public QWidget
{
    Q_OBJECT

public:

    explicit PopupWindow(
        Database &database,
        QWidget *parent = nullptr
    );

    /**
     * @brief Shows the popup near the current mouse cursor.
     */
    void showPopup();

    /**
     * @brief Reloads clipboard history from SQLite.
     */
    void refreshHistory();

protected:

    /**
     * @brief Paints the popup background.
     *
     * The main background is painted manually rather than relying
     * on a stylesheet. This gives reliable rendering for the
     * translucent frameless window.
     */
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

    void setupUi();

    void setupStyle();

    void loadHistory();

    void expandHistoryItem(
        QListWidgetItem *item
    );

    QString createPreview(
        const QString &content
    ) const;

    QScreen *activeScreen() const;

    QPoint calculatePopupPosition(
        QScreen *screen
    ) const;

private:

    /*
     * Shared application database.
     *
     * PopupWindow does not own this object.
     */
    Database &m_database;

    /*
     * Popup header.
     */
    QWidget *m_header;

    QLabel *m_titleLabel;

    QPushButton *m_settingsButton;
    QPushButton *m_minimizeButton;
    QPushButton *m_closeButton;

    /*
     * Main popup widgets.
     */
    QLineEdit *m_searchBox;
    QListWidget *m_historyList;

    /*
     * Popup dragging state.
     */
    QPoint m_dragOffset;
    bool m_dragging;
};

#endif
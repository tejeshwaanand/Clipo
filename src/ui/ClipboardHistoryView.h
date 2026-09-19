#ifndef CLIPO_CLIPBOARD_HISTORY_VIEW_H
#define CLIPO_CLIPBOARD_HISTORY_VIEW_H

#include <QListWidget>

class Database;
class QListWidgetItem;

class ClipboardHistoryView : public QListWidget
{
    Q_OBJECT

public:
    explicit ClipboardHistoryView(
        Database &database,
        QWidget *parent = nullptr
    );

    void refresh();

    void filter(const QString &searchText);

    QString selectedContent() const;

    void selectNext();
    void selectPrevious();

signals:
    // Emitted when the user double-clicks a clipboard item.
    //
    // PopupWindow will handle the actual paste operation.
    void itemDoubleClicked();

private:
    void loadHistory();

    QString createPreview(const QString &content) const;

    void expandItem(QListWidgetItem *item);

    void restoreItem(QListWidgetItem *item);

private:
    Database &m_database;
    QString m_currentSearch;
};

#endif
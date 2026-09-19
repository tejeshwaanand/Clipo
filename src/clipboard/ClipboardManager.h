#ifndef CLIPO_CLIPBOARD_MANAGER_H
#define CLIPO_CLIPBOARD_MANAGER_H

#include <QObject>
#include <QString>

class QClipboard;

/**
 * @brief Monitors and provides access to the system clipboard.
 *
 * ClipboardManager is responsible for interacting with Qt's
 * clipboard abstraction. It does not store clipboard history
 * permanently; persistence will be handled by the database layer.
 *
 * Keeping clipboard handling separate from storage makes the
 * component easier to test and allows us to change the storage
 * implementation later without modifying clipboard logic.
 */
class ClipboardManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Creates a ClipboardManager.
     *
     * @param parent Optional Qt parent object.
     */
    explicit ClipboardManager(QObject *parent = nullptr);

    /**
     * @brief Returns the current text stored in the system clipboard.
     *
     * @return Current clipboard text.
     */
    QString currentText() const;

signals:

    /**
     * @brief Emitted whenever the clipboard content changes.
     *
     * The text is emitted only when the clipboard contains
     * text that Clipo can process.
     *
     * @param text Newly copied text.
     */
    void clipboardChanged(const QString &text);

private slots:

    /**
     * @brief Handles Qt's clipboard change notification.
     *
     * This function is connected to QClipboard::dataChanged().
     */
    void handleClipboardChanged();

private:

    // Qt owns the actual clipboard object. We only keep a pointer
    // to it because QApplication provides the clipboard instance.
    QClipboard *m_clipboard;
};

#endif // CLIPO_CLIPBOARD_MANAGER_H
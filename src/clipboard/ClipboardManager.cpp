#include "ClipboardManager.h"

#include <QApplication>
#include <QClipboard>

/**
 * @brief Creates the clipboard manager and starts monitoring the
 *        system clipboard.
 */
ClipboardManager::ClipboardManager(QObject *parent)
    : QObject(parent),
      m_clipboard(QApplication::clipboard())
{
    // QClipboard emits dataChanged() whenever the system clipboard
    // content changes. Connecting to the signal allows Clipo to
    // react to clipboard changes instead of continuously polling.
    //
    // Event-driven monitoring is important for keeping Clipo
    // lightweight while running in the background.
    connect(
        m_clipboard,
        &QClipboard::dataChanged,
        this,
        &ClipboardManager::handleClipboardChanged
    );
}

/**
 * @brief Returns the current clipboard text.
 */
QString ClipboardManager::currentText() const
{
    return m_clipboard->text();
}

/**
 * @brief Processes clipboard change notifications.
 */
void ClipboardManager::handleClipboardChanged()
{
    const QString text = m_clipboard->text();

    // Ignore empty clipboard contents for now.
    //
    // Later, Clipo will also support images, files and other
    // MIME types. Those will be handled separately rather than
    // converting everything into text.
    if (text.isEmpty()) {
        return;
    }

    emit clipboardChanged(text);
}
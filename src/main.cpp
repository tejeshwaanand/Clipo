#include <QApplication>
#include <QDebug>

#include "clipboard/ClipboardManager.h"

/**
 * @brief Entry point for the Clipo application.
 *
 * At this stage, Clipo runs as a simple background Qt application
 * and monitors clipboard changes. The graphical popup will be
 * introduced in a later development stage.
 */
int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    // ClipboardManager owns the clipboard monitoring logic.
    // Keeping this logic outside main() makes the application
    // easier to extend and test.
    ClipboardManager clipboardManager;

    // For the first development milestone, print clipboard
    // changes to the terminal.
    //
    // This will later be replaced/extended with the database
    // storage layer.
    QObject::connect(
        &clipboardManager,
        &ClipboardManager::clipboardChanged,
        [](const QString &text)
        {
            qDebug() << "Clipboard:" << text;
        }
    );

    qDebug() << "Clipo started.";

    return application.exec();
}
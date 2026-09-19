#include <QApplication>
#include <QDebug>

#include "clipboard/ClipboardManager.h"
#include "database/Database.h"
#include "ui/PopupWindow.h"

/**
 * @brief Entry point for Clipo.
 *
 * At this stage, the application initializes:
 *
 * - SQLite storage
 * - Clipboard monitoring
 * - Clipboard history popup
 *
 * Global keyboard shortcuts will be added in a later milestone.
 */
int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    /*
     * Set application metadata.
     *
     * Qt uses this information for platform-specific
     * application data locations.
     */
    application.setApplicationName("Clipo");
    application.setApplicationVersion("0.1.0");
    application.setOrganizationName("Clipo");

    // ---------------------------------------------------------
    // Database
    // ---------------------------------------------------------

    Database database;

    if (!database.initialize())
    {

        qCritical()
            << "Failed to initialize Clipo database.";

        return 1;
    }
    // ---------------------------------------------------------
    // Popup UI
    // ---------------------------------------------------------

    PopupWindow popup(database);
    // ---------------------------------------------------------
    // Clipboard monitoring
    // ---------------------------------------------------------

    ClipboardManager clipboardManager;

    QObject::connect(
        &clipboardManager,
        &ClipboardManager::clipboardChanged,
        [&database, &popup](const QString &text)
        {
            if (database.addEntry(text))
            {

                qDebug()
                    << "Clipboard saved:"
                    << text;

                popup.refreshHistory();
            }
        });

    

    /*
     * For now we display the popup immediately.
     *
     * This is temporary.
     *
     * Later the popup will remain hidden and will only appear
     * when the user presses Ctrl+Shift+V.
     */
    popup.showPopup();

    return application.exec();
}
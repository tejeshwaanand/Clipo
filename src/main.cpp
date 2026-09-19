#include <QApplication>
#include <QDebug>

#include "settings/SettingsManager.h"
#include "database/Database.h"
#include "clipboard/ClipboardManager.h"
#include "platform/GlobalShortcut.h"
#include "ui/PopupWindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    /*
     * =========================================================
     * Application information
     * =========================================================
     *
     * QSettings uses the organization/application information
     * to determine where settings should be stored.
     */
    app.setApplicationName("Clipo");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("Clipo");


    /*
     * =========================================================
     * Database
     * =========================================================
     *
     * Initialize the local SQLite database.
     *
     * Clipo does not use a remote server. Clipboard history
     * remains stored locally on the user's machine.
     */
    Database database;

    if (!database.initialize())
    {
        qCritical()
            << "Clipo:"
            << "Database initialization failed.";

        return 1;
    }


    /*
     * =========================================================
     * Settings
     * =========================================================
     *
     * SettingsManager handles user preferences such as:
     *
     *     - Theme
     *     - Popup opacity
     *     - Global shortcut
     *     - Popup position
     *
     * The object is created here and passed by reference to
     * PopupWindow.
     */
    SettingsManager settings;


    /*
     * =========================================================
     * Clipboard monitoring
     * =========================================================
     *
     * ClipboardManager listens for clipboard changes and
     * notifies us whenever new text is copied.
     */
    ClipboardManager clipboardManager;

    QObject::connect(
        &clipboardManager,
        &ClipboardManager::clipboardChanged,
        [&database](const QString &text)
        {
            database.addEntry(text);
        }
    );


    /*
     * =========================================================
     * Popup
     * =========================================================
     *
     * PopupWindow receives both:
     *
     *     1. Database
     *     2. SettingsManager
     *
     * PopupWindow does not own either object.
     */
    PopupWindow popup(
        database,
        settings
    );


    /*
    * =========================================================
    * Global shortcut
    * =========================================================
    *
    * GlobalShortcut listens for the configured keyboard
    * shortcut.
    */
    GlobalShortcut globalShortcut;


    /*
    * =========================================================
    * Global shortcut activation
    * =========================================================
    *
    * When the registered global shortcut is pressed,
    * GlobalShortcut emits activated().
    *
    * This opens the Clipo popup.
    */
    QObject::connect(
        &globalShortcut,
        &GlobalShortcut::activated,
        &popup,
        &PopupWindow::showPopup
    );


    /*
    * =========================================================
    * Shortcut change
    * =========================================================
    *
    * When the user changes the shortcut from Settings,
    * unregister the old shortcut and register the new one.
    */
    QObject::connect(
        &popup,
        &PopupWindow::globalShortcutChanged,
        &globalShortcut,
        [&globalShortcut](const QString &shortcut)
        {
            if (!globalShortcut.setShortcut(shortcut))
            {
                qWarning()
                    << "Clipo:"
                    << "Could not set global shortcut:"
                    << shortcut;
            }
        }
    );


    /*
    * =========================================================
    * Register global shortcut
    * =========================================================
    */
    if (!globalShortcut.registerShortcut(
            settings.globalShortcut()
        ))
    {
        qWarning()
            << "Clipo:"
            << "Could not register global shortcut:"
            << settings.globalShortcut();
    }


    /*
     * =========================================================
     * Start application
     * =========================================================
     *
     * Clipo starts hidden.
     *
     * The user opens the popup using:
     *
     *     Ctrl + Shift + V
     */
    return app.exec();
}
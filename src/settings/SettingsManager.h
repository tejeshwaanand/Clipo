#ifndef CLIPO_SETTINGS_MANAGER_H
#define CLIPO_SETTINGS_MANAGER_H

#include <QString>

/**
 * @brief Central manager for all Clipo application settings.
 *
 * Settings are persisted using Qt's QSettings.
 *
 * Keeping settings in a separate class means UI components
 * do not need to know how or where settings are stored.
 */
class SettingsManager
{
public:
    SettingsManager();

    // ---------------------------------------------------------
    // Theme
    // ---------------------------------------------------------

    QString theme() const;

    void setTheme(const QString &theme);

    // ---------------------------------------------------------
    // Popup opacity
    // ---------------------------------------------------------

    double popupOpacity() const;

    void setPopupOpacity(double opacity);

    // ---------------------------------------------------------
    // Global shortcut
    // ---------------------------------------------------------

    QString globalShortcut() const;

    void setGlobalShortcut(const QString &shortcut);

    // ---------------------------------------------------------
    // Popup position
    // ---------------------------------------------------------

    QString popupPosition() const;

    void setPopupPosition(const QString &position);

private:
    /*
     * Default values are kept here so there is one central
     * place for Clipo's default configuration.
     */
    static constexpr const char *DEFAULT_THEME = "System";

    static constexpr double DEFAULT_POPUP_OPACITY = 0.88;

    static constexpr const char *DEFAULT_GLOBAL_SHORTCUT =
        "Ctrl+Shift+V";

    static constexpr const char *DEFAULT_POPUP_POSITION =
        "Near Cursor";
};

#endif
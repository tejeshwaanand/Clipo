#include "SettingsManager.h"
#include <algorithm>
#include <QSettings>

/**
 * @brief Creates the settings manager.
 */
SettingsManager::SettingsManager()
{
}

/**
 * @brief Returns the currently selected theme.
 */
QString SettingsManager::theme() const
{
    QSettings settings;

    return settings.value(
        "appearance/theme",
        DEFAULT_THEME
    ).toString();
}

/**
 * @brief Saves the selected theme.
 */
void SettingsManager::setTheme(
    const QString &theme
)
{
    QSettings settings;

    settings.setValue(
        "appearance/theme",
        theme
    );
}

/**
 * @brief Returns the popup opacity.
 */
double SettingsManager::popupOpacity() const
{
    QSettings settings;

    return settings.value(
        "appearance/popupOpacity",
        DEFAULT_POPUP_OPACITY
    ).toDouble();
}

/**
 * @brief Saves popup opacity.
 */
void SettingsManager::setPopupOpacity(double opacity)
{
    /*
     * Keep the popup readable.
     *
     * Minimum 75% opacity prevents the clipboard text
     * from becoming too faint.
     */
    opacity = std::clamp(
        opacity,
        0.75,
        1.00
    );

    /*
     * Use the same QSettings storage used by the
     * other SettingsManager functions.
     */
    QSettings settings;

    settings.setValue(
        "appearance/popupOpacity",
        opacity
    );
}

/**
 * @brief Returns the configured global shortcut.
 */
QString SettingsManager::globalShortcut() const
{
    QSettings settings;

    return settings.value(
        "shortcut/global",
        DEFAULT_GLOBAL_SHORTCUT
    ).toString();
}

/**
 * @brief Saves the global shortcut.
 */
void SettingsManager::setGlobalShortcut(
    const QString &shortcut
)
{
    QSettings settings;

    settings.setValue(
        "shortcut/global",
        shortcut
    );
}

/**
 * @brief Returns the popup position mode.
 */
QString SettingsManager::popupPosition() const
{
    QSettings settings;

    return settings.value(
        "popup/position",
        DEFAULT_POPUP_POSITION
    ).toString();
}

/**
 * @brief Saves the popup position mode.
 */
void SettingsManager::setPopupPosition(
    const QString &position
)
{
    QSettings settings;

    settings.setValue(
        "popup/position",
        position
    );
}
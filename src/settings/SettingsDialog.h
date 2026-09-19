#ifndef CLIPO_SETTINGS_DIALOG_H
#define CLIPO_SETTINGS_DIALOG_H

#include <QDialog>

class QComboBox;
class QDialogButtonBox;
class QKeySequenceEdit;
class QSlider;

class SettingsManager;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(
        SettingsManager &settings,
        QWidget *parent = nullptr
    );

signals:
    /**
     * @brief Emitted when appearance settings change.
     *
     * PopupWindow can use this signal to immediately
     * apply the new appearance.
     */
    void appearanceChanged();

    /**
     * @brief Emitted when the global keyboard shortcut changes.
     *
     * The main application can use this signal to unregister
     * the old shortcut and register the new one.
     */
    void globalShortcutChanged(
        const QString &shortcut
    );

private:
    void setupUi();
    void setupStyle();
    void loadSettings();
    void saveSettings();

private:
    SettingsManager &m_settings;

    QComboBox *m_themeCombo;

    QSlider *m_opacitySlider;

    /*
     * Allows the user to press a keyboard combination
     * instead of manually typing the shortcut text.
     *
     * Example:
     *
     * Ctrl + Shift + V
     */
    QKeySequenceEdit *m_shortcutEdit;

    QDialogButtonBox *m_buttonBox;
};

#endif
#include "SettingsDialog.h"

#include "SettingsManager.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QKeySequence>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>

/**
 * @brief Creates the Clipo settings dialog.
 */
SettingsDialog::SettingsDialog(
    SettingsManager &settings,
    QWidget *parent
)
    : QDialog(parent),
      m_settings(settings),
      m_themeCombo(nullptr),
      m_opacitySlider(nullptr),
      m_shortcutEdit(nullptr),
      m_buttonBox(nullptr)
{
    setWindowTitle("Clipo Settings");

    setModal(true);

    setFixedSize(
        500,
        380
    );

    setupUi();
    setupStyle();
    loadSettings();
}

/**
 * @brief Creates the settings controls.
 */
void SettingsDialog::setupUi()
{
    auto *mainLayout =
        new QVBoxLayout(this);

    mainLayout->setContentsMargins(
        12,
        8,
        12,
        12
    );

    mainLayout->setSpacing(8);

    /*
     * ---------------------------------------------------------
     * Title
     * ---------------------------------------------------------
     */

    auto *titleLabel =
        new QLabel(
            "Clipo Settings",
            this
        );

    titleLabel->setObjectName(
        "SettingsTitle"
    );

    auto *infoLabel =
        new QLabel(
            "Configure the appearance and behavior of Clipo.",
            this
        );

    infoLabel->setObjectName(
        "SettingsInfo"
    );

    /*
     * ---------------------------------------------------------
     * Settings form
     * ---------------------------------------------------------
     */

    auto *formLayout =
        new QFormLayout();

    formLayout->setSpacing(16);

    /*
     * ---------------------------------------------------------
     * Theme
     * ---------------------------------------------------------
     */

    m_themeCombo =
        new QComboBox(this);

    m_themeCombo->addItem(
        "System"
    );

    m_themeCombo->addItem(
        "Dark"
    );

    m_themeCombo->addItem(
        "Light"
    );

    /*
     * ---------------------------------------------------------
     * Popup opacity
     * ---------------------------------------------------------
     *
     * The user can select between 75% and 100%.
     */

    m_opacitySlider =
        new QSlider(
            Qt::Horizontal,
            this
        );

    m_opacitySlider->setRange(
        75,
        100
    );

    m_opacitySlider->setSingleStep(
        1
    );

    /*
     * ---------------------------------------------------------
     * Global keyboard shortcut
     * ---------------------------------------------------------
     *
     * QKeySequenceEdit allows the user to press the desired
     * keyboard combination directly.
     *
     * Example:
     *
     * Ctrl + Shift + V
     *
     * Instead of manually typing:
     *
     * Ctrl+Shift+V
     */

    m_shortcutEdit =
        new QKeySequenceEdit(this);

    /*
     * Only allow a single shortcut combination.
     *
     * Clipo currently uses one global shortcut to open
     * the popup.
     */
    // m_shortcutEdit->setMaximumSequenceLength(
    //     1
    // );

    /*
     * ---------------------------------------------------------
     * Add controls to the form
     * ---------------------------------------------------------
     */

    formLayout->addRow(
        "Theme:",
        m_themeCombo
    );

    formLayout->addRow(
        "Popup Opacity:",
        m_opacitySlider
    );

    formLayout->addRow(
        "Keyboard Shortcut:",
        m_shortcutEdit
    );

    /*
     * ---------------------------------------------------------
     * Buttons
     * ---------------------------------------------------------
     */

    m_buttonBox =
        new QDialogButtonBox(
            QDialogButtonBox::Save |
            QDialogButtonBox::Cancel,
            Qt::Horizontal,
            this
        );

    m_buttonBox->button(
        QDialogButtonBox::Save
    )->setText(
        "Save"
    );

    m_buttonBox->button(
        QDialogButtonBox::Cancel
    )->setText(
        "Cancel"
    );

    /*
     * Save button.
     */
    connect(
        m_buttonBox,
        &QDialogButtonBox::accepted,
        this,
        [this]()
        {
            /*
             * Do not allow an empty global shortcut.
             *
             * Clipo needs a shortcut to open the popup.
             */
            if (
                m_shortcutEdit->keySequence().isEmpty()
            )
            {
                QMessageBox::warning(
                    this,
                    "Invalid Shortcut",
                    "Please enter a keyboard shortcut."
                );

                return;
            }

            saveSettings();

            accept();
        }
    );

    /*
     * Cancel button.
     */
    connect(
        m_buttonBox,
        &QDialogButtonBox::rejected,
        this,
        &QDialog::reject
    );

    /*
     * ---------------------------------------------------------
     * Main layout
     * ---------------------------------------------------------
     */

    mainLayout->addWidget(
        titleLabel
    );

    mainLayout->addWidget(
        infoLabel
    );

    mainLayout->addSpacing(
        10
    );

    mainLayout->addLayout(
        formLayout
    );

    mainLayout->addStretch();

    mainLayout->addWidget(
        m_buttonBox
    );
}

/**
 * @brief Loads persisted settings into the UI.
 */
void SettingsDialog::loadSettings()
{
    /*
     * ---------------------------------------------------------
     * Theme
     * ---------------------------------------------------------
     */

    const QString currentTheme =
        m_settings.theme();

    const int themeIndex =
        m_themeCombo->findText(
            currentTheme
        );

    if (themeIndex >= 0)
    {
        m_themeCombo->setCurrentIndex(
            themeIndex
        );
    }

    /*
     * ---------------------------------------------------------
     * Popup opacity
     * ---------------------------------------------------------
     *
     * Convert:
     *
     * 0.88 -> 88
     */

    const int opacity =
        static_cast<int>(
            m_settings.popupOpacity() * 100.0
        );

    m_opacitySlider->setValue(
        opacity
    );

    /*
     * ---------------------------------------------------------
     * Global keyboard shortcut
     * ---------------------------------------------------------
     *
     * Read the saved string:
     *
     * "Ctrl+Shift+V"
     *
     * and convert it into a QKeySequence.
     */

    const QString shortcut =
        m_settings.globalShortcut();

    const QKeySequence keySequence =
        QKeySequence::fromString(
            shortcut,
            QKeySequence::PortableText
        );

    m_shortcutEdit->setKeySequence(
        keySequence
    );
}

/**
 * @brief Saves the settings currently selected by the user.
 */
void SettingsDialog::saveSettings()
{
    /*
     * ---------------------------------------------------------
     * Save theme
     * ---------------------------------------------------------
     */

    m_settings.setTheme(
        m_themeCombo->currentText()
    );

    /*
     * ---------------------------------------------------------
     * Save opacity
     * ---------------------------------------------------------
     */

    const double opacity =
        m_opacitySlider->value() / 100.0;

    m_settings.setPopupOpacity(
        opacity
    );

    /*
     * ---------------------------------------------------------
     * Save global keyboard shortcut
     * ---------------------------------------------------------
     *
     * Convert the QKeySequence into a portable string.
     *
     * Example:
     *
     * QKeySequence
     *      ↓
     * "Ctrl+Alt+V"
     */

    const QString shortcut =
        m_shortcutEdit
            ->keySequence()
            .toString(
                QKeySequence::PortableText
            );

    m_settings.setGlobalShortcut(
        shortcut
    );

    /*
     * Notify the application that the appearance changed.
     */
    emit appearanceChanged();

    /*
     * Notify the application that the global shortcut
     * changed.
     *
     * The GlobalShortcut object will handle registering
     * the new shortcut.
     */
    emit globalShortcutChanged(
        shortcut
    );
}

/**
 * @brief Applies the settings dialog style.
 */
void SettingsDialog::setupStyle()
{
    setStyleSheet(R"(
        QDialog {
            background: #1e2024;
            color: #ffffff;
        }

        QLabel#SettingsTitle {
            color: #ffffff;
            font-size: 19px;
            font-weight: 600;
        }

        QLabel#SettingsInfo {
            color: rgba(255, 255, 255, 165);
            font-size: 14px;
        }

        QFormLayout QLabel {
            color: rgba(255, 255, 255, 190);
            font-size: 14px;
        }

        QComboBox {
            min-height: 32px;

            padding: 4px 10px;

            background: rgba(255, 255, 255, 25);
            border: 1px solid rgba(255, 255, 255, 60);
            border-radius: 7px;

            color: #ffffff;
        }

        QComboBox:hover {
            background: rgba(255, 255, 255, 35);
        }

        QComboBox QAbstractItemView {
            background: #1e2024;
            color: #ffffff;
            selection-background-color: rgba(70, 140, 210, 150);
        }

        /*
        * -----------------------------------------------------
        * Keyboard shortcut editor
        * -----------------------------------------------------
        *
        * QKeySequenceEdit contains an internal QLineEdit.
        *
        * The internal QLineEdit is styled explicitly so that
        * the shortcut field matches the other dark controls
        * in the settings dialog.
        */
        QKeySequenceEdit {
            min-height: 32px;
            background: transparent;
            border: none;
        }

        /*
        * Actual input field inside QKeySequenceEdit.
        */
        QKeySequenceEdit QLineEdit {
            min-height: 32px;
            padding: 4px 10px;

            background: rgba(255, 255, 255, 25);
            border: 1px solid rgba(255, 255, 255, 60);
            border-radius: 7px;

            color: #ffffff;

            font-size: 14px;
            font-weight: 400;
        }

        /*
        * Highlight the field when it is active.
        */
        QKeySequenceEdit QLineEdit:focus {
            background: rgba(255, 255, 255, 30);
            border: 1px solid rgba(255, 255, 255, 120);
        }

        /*
        * Text selection inside the shortcut field.
        */
        QKeySequenceEdit QLineEdit {
            selection-background-color: rgba(80, 140, 220, 150);
            selection-color: #ffffff;
        }

        QSlider::groove:horizontal {
            height: 5px;
            background: rgba(255, 255, 255, 50);
            border-radius: 2px;
        }

        QSlider::handle:horizontal {
            width: 14px;
            margin: -5px 0;

            background: #ffffff;
            border-radius: 7px;
        }

        QDialogButtonBox QPushButton {
            min-width: 90px;
            min-height: 32px;

            padding: 6px 14px;

            background: rgba(255, 255, 255, 25);
            border: 1px solid rgba(255, 255, 255, 60);
            border-radius: 7px;

            color: #ffffff;
        }

        QDialogButtonBox QPushButton:hover {
            background: rgba(255, 255, 255, 45);
        }
    )");
}
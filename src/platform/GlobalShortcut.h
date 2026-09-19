#ifndef CLIPO_GLOBAL_SHORTCUT_H
#define CLIPO_GLOBAL_SHORTCUT_H

#include <QObject>
#include <QString>

class QSocketNotifier;

/**
 * @brief Provides Clipo's global keyboard shortcut.
 *
 * The shortcut is configurable through Clipo Settings.
 *
 * Example:
 *
 *     Ctrl+Shift+V
 *     Ctrl+Alt+V
 *     Super+V
 *
 * The platform-specific implementation handles the actual
 * global keyboard registration.
 */
class GlobalShortcut : public QObject
{
    Q_OBJECT

public:
    explicit GlobalShortcut(QObject *parent = nullptr);
    ~GlobalShortcut() override;

    /**
     * @brief Registers the specified global shortcut.
     *
     * @param shortcut Shortcut in Qt portable format.
     *
     * Example:
     *
     *     "Ctrl+Shift+V"
     *
     * @return true if registration succeeds.
     */
    bool registerShortcut(
        const QString &shortcut
    );

    /**
     * @brief Changes the currently registered shortcut.
     *
     * The old shortcut is unregistered first and the new
     * shortcut is then registered.
     *
     * @param shortcut New shortcut in Qt portable format.
     *
     * @return true if the new shortcut was registered.
     */
    bool setShortcut(
        const QString &shortcut
    );

    /**
     * @brief Unregisters the current global shortcut.
     */
    void unregisterShortcut();

    /**
     * @brief Returns the currently configured shortcut.
     */
    QString shortcut() const;

signals:

    /**
     * @brief Emitted when the configured global shortcut
     * is pressed.
     */
    void activated();

private:

    /**
     * @brief Processes pending X11 events.
     */
    void processEvents();

    /*
     * Currently registered shortcut.
     *
     * Example:
     *
     *     "Ctrl+Shift+V"
     */
    QString m_shortcut;

    QSocketNotifier *m_socketNotifier;
};

#endif
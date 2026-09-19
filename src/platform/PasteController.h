#ifndef CLIPO_PASTE_CONTROLLER_H
#define CLIPO_PASTE_CONTROLLER_H

#include <QString>

/**
 * @brief Platform-independent interface for paste operations.
 *
 * The UI should not contain Linux/X11-specific code.
 *
 * Current implementation:
 *     Linux + X11
 *
 * Future implementations:
 *     Linux + Wayland
 *     Windows
 */
class PasteController
{
public:

    /**
     * @brief Remembers the application that is currently active.
     *
     * This must be called BEFORE Clipo becomes active.
     *
     * @return true if an active application was found.
     */
    static bool rememberActiveWindow();

    /**
     * @brief Pastes text into the previously active application.
     *
     * @param text Complete clipboard text.
     *
     * @return true if paste was successfully initiated.
     */
    static bool paste(
        const QString &text
    );

    /**
     * @brief Clears the remembered application.
     */
    static void clearActiveWindow();
};

#endif
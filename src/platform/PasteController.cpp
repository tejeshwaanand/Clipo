#include "PasteController.h"


#if defined(Q_OS_LINUX)

/*
 * Linux/X11 implementation.
 *
 * Implemented in:
 *
 *     linux/X11PasteController.cpp
 */
bool rememberActiveWindowX11();

bool pasteUsingX11(
    const QString &text
);

void clearActiveWindowX11();

#endif


bool PasteController::rememberActiveWindow()
{
#if defined(Q_OS_LINUX)

    return rememberActiveWindowX11();

#else

    return false;

#endif
}


bool PasteController::paste(
    const QString &text
)
{
    if (text.isEmpty()) {
        return false;
    }


#if defined(Q_OS_LINUX)

    return pasteUsingX11(text);

#else

    Q_UNUSED(text);

    return false;

#endif
}


void PasteController::clearActiveWindow()
{
#if defined(Q_OS_LINUX)

    clearActiveWindowX11();

#endif
}
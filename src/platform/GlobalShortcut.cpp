#include "GlobalShortcut.h"

/*
 * This file intentionally contains only the common interface
 * implementation.
 *
 * The actual global keyboard registration is platform-specific.
 *
 * Linux/X11:
 *
 *     src/platform/linux/X11GlobalShortcut.cpp
 *
 * Windows:
 *
 *     A future Windows implementation can be added separately.
 *
 * Keeping the platform-specific implementation separate allows
 * the rest of Clipo to use GlobalShortcut without knowing how
 * the operating system implements global shortcuts.
 */
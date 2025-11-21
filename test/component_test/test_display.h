#ifndef TEST_DISPLAY_H
#define TEST_DISPLAY_H

#include <GxEPD2_3C.h>

// Declare the display object as an external variable.
// This tells other files that 'display' exists somewhere else.
extern GxEPD2_3C<GxEPD2_154_Z90c, GxEPD2_154_Z90c::HEIGHT> display;

#endif // TEST_DISPLAY_H

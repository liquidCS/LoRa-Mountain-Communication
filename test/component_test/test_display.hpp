#ifndef TEST_DISPLAY_H
#define TEST_DISPLAY_H

#include <GxEPD2_3C.h>

// Declare the display object as an external variable.
// This tells other files that 'display' exists somewhere else.
GxEPD2_3C<GxEPD2_154_Z90c, GxEPD2_154_Z90c::HEIGHT> display(GxEPD2_154_Z90c(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEH0154Z90 200x200, SSD1681

#endif // TEST_DISPLAY_H

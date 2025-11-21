#include "test_display.h"

// Define the display object here, and only here.
// This is the single instance that all tests will share.
GxEPD2_3C<GxEPD2_154_Z90c, GxEPD2_154_Z90c::HEIGHT> display(GxEPD2_154_Z90c(/*CS=10*/ 10, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEH0154Z90 200x200, SSD1681

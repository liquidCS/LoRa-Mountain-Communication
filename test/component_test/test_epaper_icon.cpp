// #include <Arduino.h>
// #include <GxEPD2_3C.h>
// #include <Fonts/FreeMonoBold9pt7b.h>
#include <icon.h>
#include "test_display.h"
// #include "mountain.h" //載入圖片的 C array
// #include "gps_icon.h" //載入gps_icon的 C array
// #include "no_location.h" //載入no gps 訊號的 C array
// #include "satellite_icon.h" //載入衛星圖示的 C array
// #include "ban_icon.h" //載入禁止圖示的 C array

// The 'display' object is now defined in test_display.cpp

// Functions
void ePaperInit2();


// Main ePaper test function
void test_epaper_icon_functionality() {

  // Init e-paper Display
  ePaperInit2();

  // Back Ground
  display.fillScreen(GxEPD_WHITE);
  
  // Draw Icons
  drawIcon(display, IconType::MOUNTAIN_BACKGROUND, 0, 0);
  // drawIcon(display, IconType::SATELLITE_SIGNAL_GOOD, 10, 10);
  // drawIcon(display, IconType::GPS_LOCATION_GOOD, 40, 10);
  drawIcon(display, IconType::GPS_LOCATION_BAD, 70, 30);
  // drawIcon(display, IconType::SATELLITE_SIGNAL_BAD, 100, 10);

  // Refresh screen
  display.display();

  // Turn off power to protect display
  display.powerOff();
  Serial.println("Poweroff e-paper display");
}

void ePaperInit2() {
  display.init(115200, true, 2, false);
  display.setRotation(1);
  // display.setFont(&FreeMonoBold9pt7b);
}



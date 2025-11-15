#include <Arduino.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>

// Display Selection 3-color 200x200
GxEPD2_3C<GxEPD2_154_Z90c, GxEPD2_154_Z90c::HEIGHT> display(GxEPD2_154_Z90c(/*CS=10*/ 10, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEH0154Z90 200x200, SSD1681

// Functions
void ePaperInit();
void ePaperPrintText(const char *, int, uint32_t, uint32_t);


// Main ePaper test function
void test_epaper_functionality() {

  // Init e-paper Display
  ePaperInit();

  // Back Ground
  display.fillScreen(GxEPD_WHITE);
  // Draw Circles
  display.fillCircle(0, 0, 100, GxEPD_BLACK);
  display.fillCircle(200, 0, 100, GxEPD_RED);
  display.fillCircle(0, 200, 100, GxEPD_RED);
  display.fillCircle(200, 200, 100, GxEPD_BLACK);
  // Print Texts
  ePaperPrintText("This is red.", 0, GxEPD_RED, GxEPD_WHITE);
  ePaperPrintText("This is black.", 1, GxEPD_BLACK, GxEPD_WHITE);
  ePaperPrintText("This is white.", 2, GxEPD_WHITE, GxEPD_BLACK);
  ePaperPrintText("This is red.", 3, GxEPD_RED, GxEPD_BLACK);
  ePaperPrintText("This is black.", 4, GxEPD_BLACK, GxEPD_RED);
  ePaperPrintText("This is white.", 5, GxEPD_WHITE, GxEPD_RED);


  // Refresh screen
  display.display();

  // Turn off power to protect display
  display.powerOff();
  Serial.println("Poweroff e-paper display");
}

void ePaperInit() {
  display.init(115200, true, 2, false);
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
}


const uint16_t padding = 2;
const int line_y_offset[] = {-50, -30, -10, 10, 30, 50};
void ePaperPrintText(const char *text, int line, uint32_t textColor, uint32_t backgroundColor)
{
  display.setTextColor(textColor);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby + line_y_offset[line];
  display.setCursor(x, y);
  display.fillRect(x+tbx-padding, y+tby-padding, tbw+padding*2, tbh+padding*2, backgroundColor);
  display.print(text);
}
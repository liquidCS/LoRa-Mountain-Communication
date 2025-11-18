#include <Arduino.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "mountain.h" //載入圖片的 C array
#include "gps_icon.h" //載入gps_icon的 C array
#include "no_location.h" //載入no gps 訊號的 C array
#include "satellite_icon.h" //載入衛星圖示的 C array
#include "ban_icon.h" //載入禁止圖示的 C array

// Display Selection 3-color 200x200
GxEPD2_3C<GxEPD2_154_Z90c, GxEPD2_154_Z90c::HEIGHT> display(GxEPD2_154_Z90c(/*CS=5*/ 10, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEH0154Z90 200x200, SSD1681

// Functions
void ePaperInit();
void ePaperPrintText(const char *, int, uint32_t, uint32_t);


// Main ePaper test function
void test_epaper_functionality() {

  // Init e-paper Display
  ePaperInit();

  // Back Ground
  display.fillScreen(GxEPD_WHITE);
  
  display.drawBitmap(
    0, 0,            // 顯示在左上角 (X=0, Y=0)
    gImage_mountain,   // 圖片陣列名稱
    200, 200,        // 圖片的寬度和高度
    GxEPD_BLACK      // 用黑色繪製
  );

  // Draw Satellite Icon
  // display.drawBitmap(
  //   0, 0,            // 顯示在 (X=0, Y=0)
  //   gImage_satellite_icon,   // 圖片陣列名稱
  //   20, 20,        // 圖片的寬度和高度
  //   GxEPD_BLACK      // 用黑色繪製
  // );

  // Draw No satellite Icon
  display.drawBitmap(
    0, 0,            // 顯示在 (X=0, Y=0)
    gImage_satellite_icon,   // 圖片陣列名稱
    20, 20,        // 圖片的寬度和高度
    GxEPD_BLACK      // 用黑色繪製
  );
  display.drawBitmap(
    0, 0,            // 顯示在 (X=0, Y=0)
    gImage_ban_icon,   // 圖片陣列名稱
    20, 20,        // 圖片的寬度和高度
    GxEPD_RED      // 用紅色繪製
  );

  // Draw GPS Icon
  // display.drawBitmap(
  //   0, 0,            // 顯示在 (X=0, Y=0)
  //   gImage_gps_icon,   // 圖片陣列名稱
  //   20, 20,        // 圖片的寬度和高度
  //   GxEPD_RED      // 用紅色繪製
  // );

  // Draw No Location Icon
  // display.drawBitmap(
  //   0, 0,            // 顯示在 (X=0, Y=0)
  //   gImage_no_location,   // 圖片陣列名稱
  //   20, 20,        // 圖片的寬度和高度
  //   GxEPD_RED      // 用紅色繪製
  // );

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
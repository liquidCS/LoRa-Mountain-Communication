#include "icon.h"

// ====================
// 函式實作 (Implementation)
// ====================

// 預設的 Icon 大小 (可以獨立定義成 const)
const int ICON_WIDTH = 20;
const int ICON_HEIGHT = 20;

// ====================
// 函式實作 (Implementation)
// ====================

// 統一的 Icon 繪圖函式 (替代了所有單獨的 draw...Icon 函式)
void drawIcon(MyDisplay &display, IconType type, int x, int y) {
    
    // 使用 switch 語句來根據 IconType 執行不同的繪圖邏輯
    switch (type) {
        
        // ----------------------------------------------------
        case IconType::MOUNTAIN_BACKGROUND:
            // 這是特殊情況，Icon 大小是 200x200
            display.drawBitmap(
                0, 0,  // 背景圖從 (0,0) 開始畫
                gImage_mountain, 
                200, 200, 
                GxEPD_BLACK
            );
            break;

        // ----------------------------------------------------
        case IconType::SATELLITE_SIGNAL_GOOD:
            display.drawBitmap(
                x, y, 
                gImage_satellite_icon, 
                ICON_WIDTH, ICON_HEIGHT, 
                GxEPD_BLACK
            );
            break;

        // ----------------------------------------------------
        case IconType::GPS_LOCATION_GOOD:
            display.drawBitmap(
                x, y, 
                gImage_gps_icon, 
                ICON_WIDTH, ICON_HEIGHT, 
                GxEPD_BLACK
            );
            break;

        // ----------------------------------------------------
        case IconType::GPS_LOCATION_BAD:
            // no_location 陣列是用紅色顯示
            display.drawBitmap(
                x, y, 
                gImage_no_location, 
                ICON_WIDTH, ICON_HEIGHT, 
                GxEPD_RED 
            );
            break;

        // ----------------------------------------------------
        case IconType::SATELLITE_SIGNAL_BAD:
            // 這是複合圖示，需要畫兩層 (黑 + 紅)
            // 1. 畫黑色衛星
            display.drawBitmap(
                x, y, 
                gImage_satellite_icon, 
                ICON_WIDTH, ICON_HEIGHT, 
                GxEPD_BLACK
            );
            // 2. 畫紅色禁止符號 (疊在上面)
            display.drawBitmap(
                x, y, 
                gImage_ban_icon, 
                ICON_WIDTH, ICON_HEIGHT, 
                GxEPD_RED
            );
            break;

        // ----------------------------------------------------
        default:
            // 處理未定義的 IconType，防止程式崩潰
            // 可以選擇畫一個空白方塊或印出除錯訊息
            Serial.println("Error: Unknown Icon Type");
            break;
    }
}

// // 實作：畫山背景
// void drawMountainBackground(MyDisplay &display) {
//   display.drawBitmap(
//     0, 0, 
//     gImage_mountain, 
//     200, 200, 
//     GxEPD_BLACK
//   );
// }

// // 實作：畫出雙色無衛星圖示 (黑色衛星 + 紅色禁止)
// void drawNoSatelliteIcon(MyDisplay &display, int x, int y) {
//   // 1. 畫黑色衛星 (位移 x, y)
//   display.drawBitmap(
//     x, y, 
//     gImage_satellite_icon, 
//     20, 20, // 假設你的 icon 大小是 20x20，請依實際調整
//     GxEPD_BLACK
//   );

//   // 2. 畫紅色禁止符號 (位移 x, y)
//   display.drawBitmap(
//     x, y, 
//     gImage_ban_icon, 
//     20, 20, 
//     GxEPD_RED
//   );
// }

// // draw good satellite signal icon
// void drawSatelliteIcon(MyDisplay &display, int x, int y) {
//   display.drawBitmap(
//     x, y, 
//     gImage_satellite_icon, 
//     20, 20, // 假設你的 icon 大小是 20x20，請依實際調整
//     GxEPD_BLACK
//   );
// }

// // draw GPS location icon
// void drawGpsIcon(MyDisplay &display, int x, int y) {
//   display.drawBitmap(
//     x, y, 
//     gImage_gps_icon, 
//     20, 20, // 假設你的 icon 大小是 20x20，請依實際調整
//     GxEPD_BLACK
//   );
// }

// // draw no GPS location icon
// void drawNoGpsIcon(MyDisplay &display, int x, int y) {
//   display.drawBitmap(
//     x, y, 
//     gImage_no_location, 
//     20, 20, // 假設你的 icon 大小是 20x20，請依實際調整
//     GxEPD_RED // 使用紅色顯示
//   );
// }

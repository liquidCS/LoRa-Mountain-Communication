#include "icon.h"

// ====================
// 建立 Icon 查找表
// ====================
const unsigned char* iconArray[] = {
    gImage_mountain,        // index 0
    gImage_satellite_icon,  // index 1
    gImage_gps_icon,        // index 2
    gImage_no_location,     // index 3
    gImage_person1,         // index 4
    gImage_satellite_icon   // index 5 (SAT_BAD 共用 satellite 底圖)
};

// 定義大小常數
const int SIZE_SMALL = 20;
const int SIZE_LARGE = 200;

// ====================
// 函式實作
// ====================
// 移除 display 參數
void drawIcon(IconType type, int x, int y, uint16_t color) {
    
    // 1. 決定圖片尺寸
    int w = SIZE_SMALL;
    int h = SIZE_SMALL;
    if (type == IconType::MOUNTAIN_BACKGROUND) {
        w = SIZE_LARGE;
        h = SIZE_LARGE;
    }
    else if (type == IconType::PERSON1) {
        w = 40;
        h = 30;
    }

    // 2. 判斷邏輯
    if (type == IconType::SATELLITE_SIGNAL_BAD) {
        // === 特殊情況：兩個icon 合併 ===
        
        // 畫底圖 (衛星) - 使用傳入顏色 (通常黑)
        display.drawBitmap(x, y, iconArray[static_cast<int>(type)], w, h, color);
        
        // 畫禁止符號 (Ban) - 強制紅
        display.drawBitmap(x, y, gImage_ban_icon, w, h, GxEPD_RED);

    } else {
        // === 一般情況 ===
        display.drawBitmap(x, y, iconArray[static_cast<int>(type)], w, h, color);
    }
}
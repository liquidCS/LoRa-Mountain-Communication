#include "icon.hpp"
#include "epaper.hpp" // 為了存取 epaper 物件

void drawIcon(IconType type, int x, int y, uint16_t color) {
		
	// 安全性檢查：避免傳入的數字超出陣列範圍 (Optional but recommended)
	if (type < 0 || type >= (sizeof(iconTable) / sizeof(iconTable[0]))) {
		return; // 超出範圍直接不畫
	}

	// --- 核心邏輯 (O(1) 存取) ---
	// 直接用 type 當作 index 取得對應的設定
	const IconDef* icon = &iconTable[type];

	// 1. 畫主圖
	epaper.display.drawBitmap(x, y, icon->data, icon->width, icon->height, color);

	// 2. 處理疊加圖 (如果有的話)
	if (icon->overlayData != NULL) {
		epaper.display.drawBitmap(x, y, icon->overlayData, icon->width, icon->height, icon->overlayColor);
	}
}
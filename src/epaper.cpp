#include "epaper.hpp"
#include "icon.hpp"

#define FREEMONO_FONT_HEIGHT 12

Epaper::Epaper()
    : display(GxEPD2_154_Z90c(/*CS=*/10, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)) {}

void Epaper::TaskScreenUpdate() {
    xTaskCreate(
        _screenRefreshLoopTask, 
        "DisplayUpdateTask", 
        10000, 
        this, 
        1, 
        NULL);
}

void Epaper::_screenRefreshLoopTask(void *parameter) {
    static_cast<Epaper *>(parameter)->_screenRefreshLoop();
}

void Epaper::_screenRefreshLoop() {
    for (;;) {
        #if ENABLE_SLEEP
        xSemaphoreTake(sleepLock, portMAX_DELAY);
        drawFullScreen();
        DEBUG_PRINT(F("Update screen"));
        //delay(5000);
        xSemaphoreGive(sleepLock);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        #else
        drawFullScreen();
        DEBUG_PRINT(F("Update screen"));
        vTaskDelay(screen_update_delay / portTICK_PERIOD_MS);
        #endif
    }
}

void Epaper::drawFullScreen() {
    _initDisplay();
    display.fillScreen(GxEPD_WHITE); // White background
    _drawMap();
    _drawTopBar();
    // draw icon
    drawIcon(ICON_PERSON_ME, 50,50, GxEPD_BLACK);
    display.display();
    display.powerOff();
}

void Epaper::clearDisplay() {
    _initDisplay();
    display.clearScreen();
    display.powerOff();
}

void Epaper::_initDisplay() {
    display.init(115200, true, 2, false);
    display.setRotation(1);
}

void Epaper::_setTopBarTimeStyle() {
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextSize(1);
    display.setTextColor(GxEPD_BLACK);
}

void Epaper::_setTopBarGPSStyle() {
    display.setFont(&Org_01);
    display.setTextSize(1);
    display.setTextColor(GxEPD_BLACK);
}

void Epaper::_drawTopBar() {
    display.drawFastHLine(0, topbar_height, display.width(), GxEPD_BLACK);
    display.drawFastHLine(0, topbar_height - 2, display.width(), GxEPD_BLACK);

    if(myDevice.time.valid)
    {
        _setTopBarTimeStyle();
        char text[10];
        snprintf(text, sizeof(text), "%02d:%02d", myDevice.time.hour, myDevice.time.minute);
        display.setCursor(0, FREEMONO_FONT_HEIGHT);
        display.print(text);
    }

    if (myDevice.location.valid) 
    {
        _setTopBarGPSStyle();
        char char_lon[15];
        char char_lat[15];
        dtostrf(myDevice.location.latitude, 10, 6, char_lat);
        dtostrf(myDevice.location.longitude, 10, 6, char_lon);

        int16_t x, y;
        uint16_t w, h;
        display.getTextBounds(char_lon, 0, 0, &x, &y, &w, &h);
        display.setCursor(display.width() - w, h);
        display.print(char_lon);

        display.getTextBounds(char_lat, 0, 0, &x, &y, &w, &h);
        display.setCursor(display.width() - w, topbar_height - 5);
        display.print(char_lat);
    }
}

void Epaper::_drawMap() {
    display.fillRect(0, topbar_height, display.width(), display.height() - topbar_height, GxEPD_RED);
}

Epaper epaper;
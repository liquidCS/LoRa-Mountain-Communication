#ifndef EPAPER_H
#define EPAPER_H

#include <Arduino.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/Org_01.h>

#include "gps.hpp"
#include "device.hpp"

#if ENABLE_SLEEP
extern SemaphoreHandle_t sleepLock;
#endif

class Epaper {
public:
    Epaper(); // Constructor
    void TaskScreenUpdate();
    void drawFullScreen();
    void clearDisplay();

private:
    static const uint16_t FREEMONO_FONT_HEIGHT = 12;
    static const uint16_t topbar_height = 20;
    static const uint16_t screen_update_delay = 60000; // ms

    GxEPD2_3C<GxEPD2_154_Z90c, GxEPD2_154_Z90c::HEIGHT> display;

    void _initDisplay();
    void _screenRefreshLoop();
    void _setTopBarTimeStyle();
    void _setTopBarGPSStyle();
    void _drawTopBar();
    void _drawMap();

    static void _screenRefreshLoopTask(void *parameter);
};

extern Epaper epaper;

#endif // EPAPER_H
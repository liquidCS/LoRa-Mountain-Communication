#ifndef EPAPER_H
#define EPAPER_H

#include <Arduino.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/Org_01.h>

#include "gps.hpp"
#include "device.hpp"
#include "map.hpp"

#if ENABLE_SLEEP
extern SemaphoreHandle_t sleepLock;
#endif

class Epaper {
public:
    Epaper(); // Constructor
    void TaskScreenUpdate();
    void drawFullScreen();
    void clearDisplay();
    GxEPD2_3C<GxEPD2_154_Z90c, GxEPD2_154_Z90c::HEIGHT> display;

private:
    static const uint16_t FREEMONO_FONT_HEIGHT = 12;
    static const uint16_t topbar_height = 20;
    #ifdef DEBUG 
    static const uint16_t screen_update_delay = 15000; // ms
    #else   
    static const uint16_t screen_update_delay = 60000; // ms
    #endif


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
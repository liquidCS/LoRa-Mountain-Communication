#include "epaper.h"
#include <Arduino.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/Org_01.h>

#define FREEMONO_FONT_HEIGHT 12

GxEPD2_3C<GxEPD2_154_Z90c, GxEPD2_154_Z90c::HEIGHT> display(GxEPD2_154_Z90c(/*CS=5*/ 10, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEH0154Z90 200x200, SSD1681
const uint16_t display_width = display.width(); // 200 
const uint16_t display_height  = display.height(); // 200
const uint16_t topbar_height = 20;

// Functions 
void _initDisplay();
void _setTopBarTimeStyle();
void _setTopBarGPSStyle();
void _drawTopBar();
void _drawMap();


void _drawTopBar() {
    display.drawFastHLine(0, topbar_height, display_width, GxEPD_BLACK);
    display.drawFastHLine(0, topbar_height-2, display_width, GxEPD_BLACK);

    _setTopBarTimeStyle();
    char text[10] = "13:43";
    display.setCursor(0, FREEMONO_FONT_HEIGHT);
    display.print(text); // Display time from GPS module; millis() is placeholder.

    _setTopBarGPSStyle();
    char char_lon[15] = "120.471837"; // Display location info from GPS module. Text is placeholder for now.
    char char_lat[15] = "23.558387";
    int16_t x, y; uint16_t w, h;
    display.getTextBounds(char_lon, 0, 0, &x, &y, &w, &h);
    display.setCursor(display_width - w, h);
    display.print(char_lon);

    display.getTextBounds(char_lat, 0, 0, &x, &y, &w, &h);
    display.setCursor(display_width - w, topbar_height-5);
    display.print(char_lat);
}

void _drawMap() {
    display.fillRect(0, topbar_height, display_width, display_height - topbar_height, GxEPD_RED); // Place holder for map canvas size
}


void drawFullScreen() {
    _initDisplay();

    display.fillScreen(GxEPD_WHITE); // White background

    _drawMap();
    _drawTopBar();

    display.display();
    display.powerOff();
}

void clearDisplay() {
    _initDisplay();    
    display.clearScreen();
    display.powerOff();
}


void _initDisplay() {
    display.init(115200, true, 2, false);
    display.setRotation(1);
}


void _setTopBarTimeStyle() {
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextSize(1);
    display.setTextColor(GxEPD_BLACK);
}

void _setTopBarGPSStyle() {
    display.setFont(&Org_01);
    display.setTextSize(1);
    display.setTextColor(GxEPD_BLACK);
}
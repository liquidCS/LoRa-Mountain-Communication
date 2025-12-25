#ifndef MAP_HPP
#define MAP_HPP

#include <Arduino.h>
#include "device.hpp"
#include "epaper.hpp"
#include "icon.hpp"
#include "config.hpp"

class Map {
public:
    Map(uint16_t xOffset, uint16_t yOffset, uint16_t width, uint16_t height,
        double xEPSGMin, double xEPSGMax, double yEPSGMin, double yEPSGMax)
        : mapXOffset(xOffset), mapYOffset(yOffset), mapWidth(width), mapHeight(height),
          mapyEPSGMin(yEPSGMin), mapyEPSGMax(yEPSGMax), mapxEPSGMin(xEPSGMin), mapxEPSGMax(xEPSGMax) {};
    void SetMapCenter(double centerEPSGX, double centerEPSGY) { mapEPSGCenterX = centerEPSGX; mapEPSGCenterY = centerEPSGY; };
    void AdjustBoundaries(double EPSGXRange = 500, double EPSGYRange = 500);
    void UpdateMap();
    void LatLngToEPSG3857(double lat, double lng, double& x, double& y);
private:
    uint16_t mapXOffset;
    uint16_t mapYOffset;
    uint16_t mapWidth;
    uint16_t mapHeight;

    double mapxEPSGMin;
    double mapxEPSGMax;
    double mapyEPSGMin;
    double mapyEPSGMax;

    double mapEPSGCenterX;
    double mapEPSGCenterY;

    double gridIntervalMeters = 100.0;

    void _latLonToXY(double lat, double lon, int* x, int* y);
    void _drawGridLines();
    void _drawDevices();
};  

extern Map myMap;

#endif
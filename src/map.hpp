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
        double latMin, double latMax, double lonMin, double lonMax)
        : mapXOffset(xOffset), mapYOffset(yOffset), mapWidth(width), mapHeight(height),
          mapLatMin(latMin), mapLatMax(latMax), mapLonMin(lonMin), mapLonMax(lonMax) {};
    void SetMapCenter(double centerLat, double centerLon) { mapCenterLat = centerLat; mapCenterLon = centerLon; };
    void AdjustBoundaries(double LatRange = 0.01, double LonRange = 0.01);
    void UpdateMap();
private:
    uint16_t mapXOffset;
    uint16_t mapYOffset;
    uint16_t mapWidth;
    uint16_t mapHeight;

    double mapLatMin;
    double mapLatMax;
    double mapLonMin;
    double mapLonMax;

    double mapCenterLat;
    double mapCenterLon;

    void _latLonToXY(double lat, double lon, int* x, int* y);
    void _drawGridLines();
    void _drawDevices();
};  

extern Map myMap;

#endif
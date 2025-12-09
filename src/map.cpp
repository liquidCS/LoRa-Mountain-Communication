#include "map.hpp"

void Map::_latLonToXY(double lat, double lon, int* x, int* y) {
    *x = mapXOffset + static_cast<int>(((lon - mapLonMin) / (mapLonMax - mapLonMin)) * mapWidth);
    *y = mapYOffset + static_cast<int>(((mapLatMax - lat) / (mapLatMax - mapLatMin)) * mapHeight);
}

void Map::_drawGridLines() {
    const uint16_t lineColor = GxEPD_BLACK;

    // Horizontal lines draw every .01 degree
    for (double lat = mapLatMin; lat <= mapLatMax; lat += 0.002) {
        int x, y;
        _latLonToXY(lat, mapLonMin, &x, &y); 
        DEBUG_PRINTF("Drawing horizontal line at y=%d for lat=%.4f\n", y, lat);
        epaper.display.drawFastHLine(mapXOffset, y, mapWidth, lineColor);
    }

    // Vertical lines
    for (double lon = mapLonMin; lon <= mapLonMax; lon += 0.002) {
        int x, y;
        _latLonToXY(mapLatMin, lon, &x, &y); 
        DEBUG_PRINTF("Drawing vertical line at x=%d for lon=%.4f\n", x, lon);
        epaper.display.drawFastVLine(x, mapYOffset, mapHeight, lineColor);
    }
}

/*
* Adjust map boundaries based on current center
*/
void Map::AdjustBoundaries(double LatRange, double LonRange) {
    mapLatMin = mapCenterLat - LatRange;
    mapLatMax = mapCenterLat + LatRange;
    mapLonMin = mapCenterLon - LonRange;
    mapLonMax = mapCenterLon + LonRange;

    DEBUG_PRINTF("New map bounds: lat[%.6f, %.6f], lon[%.6f, %.6f]\n", mapLatMin, mapLatMax, mapLonMin, mapLonMax);
}

void Map::_drawDevices() {
    // Draw other devices
    for (int i = 0; i < DEVICE_LIST_MAX_SIZE; i++) {
        OtherDevice& device = deviceList[i];
        if (device.GetStatus() != DEVICE_STATUS_UNKNOWN && device.location.valid) {
            int x, y;
            _latLonToXY(device.location.latitude, device.location.longitude, &x, &y);
            DEBUG_PRINTF("Drawing device UID=%08X at x=%d, y=%d\n", device.GetUID(), x, y);

            // TODO: Draw different icon based on device status or other different device 
            drawIcon(ICON_PERSON1, x , y , GxEPD_BLACK); // x, y is the center of the icon
        }
    }

    // Draw self device
    if (myDevice.location.valid) {
        int x, y;
        _latLonToXY(myDevice.location.latitude, myDevice.location.longitude, &x, &y);
        DEBUG_PRINTF("Drawing self device at x=%d, y=%d\n", x, y);
        drawIcon(ICON_PERSON_ME, x , y , GxEPD_RED); // x, y is the center of the icon
    }
}

void Map::UpdateMap() {
    // For Test: Create a dummy device
    if(!CheckDeviceExists(1)) {
        CreateNewDevice(1);
    } else {
        UpdateDeviceLocation(1, myDevice.location.latitude + 0.002, myDevice.location.longitude + 0.003, 0.0); 
    }
    if(myDevice.location.valid) {
        SetMapCenter(myDevice.location.latitude, myDevice.location.longitude);
        AdjustBoundaries();
    }
    _drawGridLines();
    _drawDevices();
}

Map myMap = Map(0, 20, 200, 180,  // xOffset, yOffset, width, height
          23.55, 23.56,     // latMin, latMax
          120.46, 120.49);  // lonMin, lonMax


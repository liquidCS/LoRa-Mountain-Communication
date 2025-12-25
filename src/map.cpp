#include "map.hpp"
#include <cmath>

// Convert WGS84 (lat/lng) to EPSG:3857 (Web Mercator) coordinates
void Map::LatLngToEPSG3857(double lat, double lng, double& x, double& y) {
    const double EARTH_RADIUS = 6378137.0; // Earth radius in meters 
    
    // Validate WGS84 latitude range [-90, 90]
    if (lat < -85.051129 || lat > 85.051129) {
        DEBUG_PRINTF("Latitude %.6f out of Web Mercator range [-85.051129, 85.051129]\n", lat);
        x = NAN;
        y = NAN;
        return;
    }
    
    // X coordinate: simple conversion from longitude
    x = EARTH_RADIUS * (lng * M_PI / 180.0);
    
    // y = ln(tan(π/4 + lat/2)) * EARTH_RADIUS
    double latRad = lat * M_PI / 180.0;
    y = EARTH_RADIUS * log(tan(M_PI / 4.0 + latRad / 2.0));
    
    DEBUG_PRINTF("WGS84 (lat=%.6f, lng=%.6f) -> EPSG:3857 (x=%.2f, y=%.2f)\n", lat, lng, x, y);
}

void Map::_latLonToXY(double lat, double lon, int* x, int* y) {
    double xEPSG, yEPSG;
    LatLngToEPSG3857(lat, lon, xEPSG, yEPSG);
    *x = mapXOffset + static_cast<int>(((xEPSG - mapxEPSGMin) / (mapxEPSGMax - mapxEPSGMin)) * mapWidth);
    *y = mapYOffset + mapHeight - static_cast<int>(((yEPSG - mapyEPSGMin) / (mapyEPSGMax - mapyEPSGMin)) * mapHeight);
}

void Map::_drawGridLines() {
    const uint16_t lineColor = GxEPD_BLACK;

    // Horizontal lines draw every gridIntervalMeters
    for(double EPSGY = mapyEPSGMin; EPSGY <= mapyEPSGMax; EPSGY += gridIntervalMeters) {
        double y = mapYOffset + mapHeight - ((EPSGY - mapyEPSGMin) / (mapyEPSGMax - mapyEPSGMin)) * mapHeight;
        epaper.display.drawFastHLine(mapXOffset, y, mapWidth, lineColor);
        DEBUG_PRINTF("Drawing grid line at EPSGY=%.2f -> y=%.2f\n", EPSGY, y);
    }
    // Vertical lines
    for(double EPSGX = mapxEPSGMin; EPSGX <= mapxEPSGMax; EPSGX += gridIntervalMeters) {
        double x = mapXOffset + ((EPSGX - mapxEPSGMin) / (mapxEPSGMax - mapxEPSGMin)) * mapWidth;
        epaper.display.drawFastVLine(x, mapYOffset, mapHeight, lineColor);
        DEBUG_PRINTF("Drawing grid line at EPSGX=%.2f -> x=%.2f\n", EPSGX, x);
    }
}

/*
* Adjust map boundaries based on current center
*/
void Map::AdjustBoundaries(double EPSGRangeX, double EPSGRangeY) {
    mapxEPSGMin = mapEPSGCenterX - EPSGRangeX;
    mapxEPSGMax = mapEPSGCenterX + EPSGRangeX;
    mapyEPSGMin = mapEPSGCenterY - EPSGRangeY;
    mapyEPSGMax = mapEPSGCenterY + EPSGRangeY;
    DEBUG_PRINTF("New map bounds: x[%.6f, %.6f], y[%.6f, %.6f]\n", mapxEPSGMin, mapxEPSGMax, mapyEPSGMin, mapyEPSGMax);
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
        double xEPSG, yEPSG;
        LatLngToEPSG3857(myDevice.location.latitude, myDevice.location.longitude, xEPSG, yEPSG);
        SetMapCenter(xEPSG, yEPSG);
        DEBUG_PRINTF("Map center set to EPSG:3857 (x=%.2f, y=%.2f)\n", xEPSG, yEPSG);
        AdjustBoundaries();
    }
    _drawGridLines();
    _drawDevices();
}

Map myMap = Map(0, 20, 200, 180,  // xOffset, yOffset, width, height
          2698668.739, 2699883.120,     // latMin, latMax
          13409545.860, 13412885.445);  // lonMin, lonMax


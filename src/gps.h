#include <TinyGPS++.h>
#include <Arduino.h>

#define GPS_BAUD 9600
#define GPS_TX 1 // PinTx for GPS module on ESP32 
#define GPS_RX 38 // PinRx for GPS module on EPS32


typedef struct gps_time_t
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} gps_time_t;

typedef struct location_t
{
    double latitude;
    double longitude;
} location_t;

class GPS 
{
private:
    TinyGPSPlus gpsplus;
    location_t location;
    gps_time_t time;


public:
    bool validLocation = false;
    bool validTime = false;

    GPS();
    void ReadFromGPS();

    gps_time_t GetTime() { return time; };
    location_t GetLocation() { return location; };
};

void TaskGPSUpdate();

extern GPS gps;
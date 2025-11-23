#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG 1 // Option for printing debug info in serial

// === GPS Configuration ===
#define GPS_BAUD 9600
#define GPS_TX 1 // PinTx for GPS module on ESP32 
#define GPS_RX 38 // PinRx for GPS module on EPS32
#define GPS_UPDATE_INTERVAL_MS 10000 // Update GPS data every x ms 


#if DEBUG
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(...)
#endif

#endif // CONFIG_H
#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG 1 // Option for printing debug info in serial

// === GPS Configuration ===
#define GPS_BAUD 9600
#define GPS_TX 1 // PinTx for GPS module on ESP32 
#define GPS_RX 38 // PinRx for GPS module on EPS32
#define GPS_UPDATE_INTERVAL_MS 10000 // Update GPS data every x ms 

// === Device Configuration ===
#define DEVICE_ID_MAX_LENGTH 16
#define DEVICE_LIST_MAX_SIZE 16 // Max 256 devices in the list

// === LoRa Configuration ===
// 硬體腳位定義
#define LORA_CS   5
#define LORA_DIO1 6
#define LORA_RST  3
#define LORA_BUSY 9
#define LORA_SCK  12
#define LORA_MISO 13
#define LORA_MOSI 11

// === Sleep Configuration ===
#define ENABLE_SLEEP 0

#ifndef DEBUG
#define LORA_RANDOM_INTERVAL_BASE_MS 52000 // Base interval for LoRa sending
#define LORA_RANDOM_INTERVAL_JITTER_MS 10000 // Jitter for LoRa sending interval *2
#else
#define LORA_RANDOM_INTERVAL_BASE_MS 5000 // Base interval for LoRa sending
#define LORA_RANDOM_INTERVAL_JITTER_MS 1000 // Jitter for LoRa sending interval
#endif


#if DEBUG
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
    #define DEBUG_FLUSH() Serial.flush()
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(...)
#endif

#endif // CONFIG_H
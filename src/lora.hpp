#ifndef LORA_HPP
#define LORA_HPP

#include <Arduino.h>
#include <LoraMesher.h>
#include "device.hpp"
#include "gps.hpp"

// 硬體腳位定義
#define LORA_CS   5
#define LORA_DIO1 6
#define LORA_RST  3
#define LORA_BUSY 9
#define LORA_SCK  12
#define LORA_MISO 13
#define LORA_MOSI 11

// 封包結構定義
struct __attribute__((packed)) NodeData {
    uint8_t appId;
    uint16_t nodeId;    
    float lat;          
    float lon;          
    uint8_t battery;    
    bool isSOS;         
};

//定義APP ID
#define MOUNTAIN_APP_ID  0x88

// 函式宣告
void TaskLoraStart();
void initLoRaMesh();
void processReceivedPackets(void* pvParameters); // 接收任務
void TaskLoRaSender(void *pvParameters);         // 發送任務

extern LoraMesher& radio;
#endif
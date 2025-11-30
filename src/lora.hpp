#ifndef LORA_HPP
#define LORA_HPP

#include <Arduino.h>
#include <LoraMesher.h>
#include "config.hpp"
#include "device.hpp"
#include "gps.hpp"

#if ENABLE_SLEEP
extern SemaphoreHandle_t sleepLock;
#endif


// 封包結構定義
struct __attribute__((packed)) NodeData {
    uint8_t appId;
    uint32_t nodeId;    
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
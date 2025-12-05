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
// GPS 資料
struct __attribute__((packed)) NodeData {
    uint8_t appId;
    uint32_t nodeId;    
    float lat;          
    float lon;          
    uint8_t battery;    
    bool isSOS;         
};

// 請求封包 (A 問 B)
struct __attribute__((packed)) NameReqPacket {
    uint8_t appId;      // 0x89
    uint16_t targetId;  // 指定目標UID
};

// 回覆封包 (B 回答 A)
struct __attribute__((packed)) NameResPacket {
    uint8_t appId;      // 0x90
    uint16_t srcId;     // UID
    char name[DEVICE_ID_MAX_LENGTH];      // 自定義ID
};

//定義APP ID
#define MOUNTAIN_APP_ID  0x88  // GPS資料
#define APP_ID_NAME_REQ  0x89  // 詢問名字請求
#define APP_ID_NAME_RES  0x90  // 回覆名字

// 函式宣告
void TaskLoraStart();
void initLoRaMesh();
void processReceivedPackets(void* pvParameters); // 接收任務
void TaskLoRaSender(void *pvParameters);         // 發送任務
void sendNameRequest(uint16_t targetId);
void sendNameResponse();

extern LoraMesher& radio;
#endif
#ifndef NODES_HPP
#define NODES_HPP

#include <Arduino.h>
#include <vector>     
#include "lora.hpp"

// 遠端裝置結構
struct RemoteNode {
    NodeData data;          // 包含 ID, GPS, 電量, SOS
    uint32_t lastReceived;  // 最後收到訊號的時間 (ms)
    //int rssi;               // 訊號強度
};

//初始化 (建立 Mutex 鎖)
void initNodeManager();

// 更新或新增節點 (當收到 LoRa 封包時呼叫)
void updateRemoteNode(NodeData newData);

//取得所有節點清單 (給 E-Paper 顯示用)
std::vector<RemoteNode>& getNodesList();

//互斥鎖 (保護多工存取)
extern SemaphoreHandle_t nodeMutex;

#endif
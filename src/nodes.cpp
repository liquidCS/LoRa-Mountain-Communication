#include "nodes.hpp"

// 名冊清單
std::vector<RemoteNode> nodesList;

// 互斥鎖 (防止 LoRa 寫入時，螢幕剛好讀取導致崩潰)
SemaphoreHandle_t nodeMutex;

void initNodeManager() {
    nodeMutex = xSemaphoreCreateMutex();
}

// 收到資料後的處理
void updateRemoteNode(NodeData newData) {
    // 上鎖
    if (xSemaphoreTake(nodeMutex, portMAX_DELAY) == pdTRUE) {
        
        bool found = false;

        // 查看這個 ID 是否已經在名單裡
        for (auto &node : nodesList) {
            if (node.data.nodeId == newData.nodeId) {
                // 更新資料
                node.data = newData;       // 更新座標、電量等
                node.lastReceived = millis(); // 更新時間戳記
                //node.rssi = rssi;
                found = true;
                
                Serial.printf("[Nodes] Updated Node: 0x%X\n", newData.nodeId);
                break;
            }
        }

        // 新隊員
        if (!found) {
            RemoteNode newNode;
            newNode.data = newData;
            newNode.lastReceived = millis();
            //newNode.rssi = rssi;
            
            nodesList.push_back(newNode); // 加入清單末尾
            
            Serial.printf("[Nodes] New Node Added: 0x%X\n", newData.nodeId);
        }

        // 解鎖
        xSemaphoreGive(nodeMutex);
    }
}

// 取得清單 (給顯示器用)
std::vector<RemoteNode>& getNodesList() {
    return nodesList;
}
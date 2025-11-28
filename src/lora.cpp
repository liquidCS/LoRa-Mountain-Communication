#include "lora.hpp"
#include "nodes.hpp"

LoraMesher& radio = LoraMesher::getInstance();

uint8_t myBat = 100;
bool mySOS = false;

// 定義接收任務的 Handle (給 LoRaMesher 用的)
TaskHandle_t receiveTaskHandle = NULL;

// 初始化函式
void TaskLoraStart(){
    initLoRaMesh();
    initNodeManager();
    xTaskCreate(
        TaskLoRaSender,  // 任務函式
        "LoRaSender",    // 任務名稱
        20480,            // Stack 大小
        NULL,            // 參數
        1,               // 優先級
        NULL             // Handle
    );
}
void initLoRaMesh() {
    DEBUG_PRINTLN("[LoRa] Initializing...");
    //gpio_install_isr_service(0);
    LoraMesher::LoraMesherConfig config; 

    // 設定腳位
    config.loraCs = LORA_CS;
    config.loraRst = LORA_RST;
    
    // IRQ 對應 SX1262 的 DIO1
    config.loraIrq = LORA_DIO1; // 填入 6
    
    //在 SX1262 的建構子中，這個位置對應 BUSY 腳位, 雖然變數叫 Io1，但填入 BUSY 腳位
    config.loraIo1 = LORA_BUSY;

    // 設定參數
    config.freq = 915.0; 
    config.sf = 11;
    config.bw = 125.0;
    
    // 設定模組型號
    config.module = LoraMesher::LoraModules::SX1262_MOD; 

    // 啟動 Radio
    radio.begin(config);

    //手動建立接收任務並註冊
    int res = xTaskCreate(
        processReceivedPackets,  // 接收函式
        "Receive App Task",      // 任務名稱
        20480,                    // Stack 大小
        NULL,                    // 參數
        2,                       // 優先級
        &receiveTaskHandle       // 回傳 Handle
    );

    if (res == pdPASS) {
        // 告訴LoRaMesher收到信請通知這個任務
        radio.setReceiveAppDataTaskHandle(receiveTaskHandle);
    } else {
        DEBUG_PRINTLN("[Error] Failed to create Receive Task");
    }

    // 開始運作
    radio.start();

    DEBUG_PRINTF("[LoRa] Started! Device ID: 0x%X\n", radio.getLocalAddress());
}

// 接收任務
void processReceivedPackets(void*) {
    for (;;) {
        // 等待通知 (Blocking), LoRaMesher 會發送通知叫醒這個任務
        ulTaskNotifyTake(pdPASS, portMAX_DELAY);

        // 檢查佇列中是否有封包
        while (radio.getReceivedQueueSize() > 0) {
            
            // 取出封包
            AppPacket<uint8_t>* packet = radio.getNextAppPacket<uint8_t>();
            
            
            if (packet) {
                // 解析數據
                NodeData* receivedData = (NodeData*)packet->payload;
                if (receivedData->appId != MOUNTAIN_APP_ID) {
                    DEBUG_PRINTF("[RX] Ignored packet from 0x%X (Wrong AppID: 0x%X)\n", 
                                    packet->src, receivedData->appId);
                    
                    // 這是別人的封包或是雜訊，直接刪除，不處理
                    radio.deletePacket(packet);
                    continue; // 跳過這次迴圈，處理下一個封包
                }
                updateRemoteNode(*receivedData);
                DEBUG_PRINTF("     From UID: 0x%X |  GPS: %.6f, %.6f | Bat: %d%% | SOS: %d | Dis: %.2f m \n", 
                              receivedData->nodeId,
                              receivedData->lat, receivedData->lon, 
                              receivedData->battery, receivedData->isSOS,
                              TinyGPSPlus::distanceBetween(myDevice.location.latitude, myDevice.location.longitude, receivedData->lat, receivedData->lon)
                            );

                // 刪除封包
                radio.deletePacket(packet);
            }
        }
    }
}

// 發送任務
void TaskLoRaSender(void *pvParameters) {
    uint32_t nextInterval;

    for (;;) 
    {
        NodeData payload;
        uint64_t fullUID = myDevice.GetUID();
        payload.appId = MOUNTAIN_APP_ID;
        payload.nodeId = (uint16_t)(fullUID & 0xFFFF);
        if (myDevice.location.valid) { 
            payload.lat = myDevice.location.latitude;
            payload.lon = myDevice.location.longitude;
        } else {
            payload.lat = 0.0;
            payload.lon = 0.0;
        }
        payload.battery = myBat;
        payload.isSOS = mySOS;

        DEBUG_PRINT("[TX] Sending Packet... ");
        
        // 參數解釋：目標, 資料指標, 資料"數量"(不是Bytes)
        // 因為用了 <NodeData> 模板，最後一個參數填 1 (代表 1 個 NodeData)
        radio.createPacketAndSend<NodeData>(BROADCAST_ADDR, &payload, 1);
        
        
        DEBUG_PRINTLN("Sent!");

        // 隨機間隔邏輯
        if (payload.isSOS) {
            nextInterval = random(5000, 8000);
        } else {
            uint32_t base = LORA_RANDOM_INTERVAL_BASE_MS;
            uint32_t idOffset = (payload.nodeId & 0x0F) * 1000; 
            int32_t jitter = random(-LORA_RANDOM_INTERVAL_JITTER_MS, LORA_RANDOM_INTERVAL_JITTER_MS);
            nextInterval = base + idOffset + jitter;
        }

        //myLat += 0.0001;  //測試用, 模擬移動
        vTaskDelay(nextInterval / portTICK_PERIOD_MS);
    }
}
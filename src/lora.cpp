#include "lora.hpp"

LoraMesher& radio = LoraMesher::getInstance();

uint8_t myBat = 100;
bool mySOS = false;

// 定義接收任務的 Handle (給 LoRaMesher 用的)
TaskHandle_t receiveTaskHandle = NULL;

// 初始化函式
void TaskLoraStart(){
    gpio_wakeup_enable((gpio_num_t)LORA_DIO1, GPIO_INTR_HIGH_LEVEL);    //設定 DIO1 為喚醒源
    esp_sleep_enable_gpio_wakeup();

    initLoRaMesh();
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
                uint8_t currentAppId = packet->payload[0];
                if (currentAppId == MOUNTAIN_APP_ID) {
                    // 解析數據
                    NodeData* receivedData = (NodeData*)packet->payload;
                    if (receivedData->appId != MOUNTAIN_APP_ID) {
                        DEBUG_PRINTF("[RX] Ignored packet from 0x%X (Wrong AppID: 0x%X)\n", 
                                        packet->src, receivedData->appId);
                        
                        // 這是別人的封包或是雜訊，直接刪除，不處理
                        radio.deletePacket(packet);
                        continue; // 跳過這次迴圈，處理下一個封包
                    }

                    // Check if device exists, if not create new 
                    if(!CheckDeviceExists(receivedData->nodeId)) {
                        if(CreateNewDevice(receivedData->nodeId) == DEVICE_LIST_MAX_SIZE) { // Failed to create new device
                            DEBUG_PRINTF("Failed to create new device for UID: 0x%X\n", receivedData->nodeId);
                            continue;
                        }
                        DEBUG_PRINTF("[RX] New Device %04X detected! Sending Name Req...\n", receivedData->nodeId);
                        sendNameRequest(receivedData->nodeId);
                    }

                    UpdateDeviceLocation(receivedData->nodeId, receivedData->lat, receivedData->lon, 0.0);
                    
                    
                    #if CHECK_UNKNOW_DEVICE
                    if (Device::IsDeviceIDValid(receivedData->nodeId)) {
                        DEBUG_PRINTF("[RX] Known Node 0x%04X but name unknown. Retrying request...\n", receivedData->nodeId);
                        sendNameRequest(receivedData->nodeId);
                    }
                    #endif
                    DEBUG_PRINTF("     From UID: 0x%X |  GPS: %.6f, %.6f | Bat: %d%% | SOS: %d | Dis: %.2f m \n", 
                                receivedData->nodeId,
                                receivedData->lat, receivedData->lon, 
                                receivedData->battery, receivedData->isSOS,
                                TinyGPSPlus::distanceBetween(myDevice.location.latitude, myDevice.location.longitude, receivedData->lat, receivedData->lon)
                                );
                }else if (currentAppId == APP_ID_NAME_REQ) {
                    NameReqPacket* req = (NameReqPacket*)packet->payload;
                    uint16_t myId16 = (uint16_t)(myDevice.GetUID() & 0xFFFF);

                    if (req->targetId == myId16) {
                        DEBUG_PRINTLN("[RX] Someone asked my name!");
                        sendNameResponse();
                    }
                }else if (currentAppId == APP_ID_NAME_RES) {
                    NameResPacket* res = (NameResPacket*)packet->payload;
                    
                    DEBUG_PRINTF("[RX] Node 0x%04X is named: %s\n", res->srcId, res->name);
                    
                    char tempName[DEVICE_ID_MAX_LENGTH];
                    strncpy(tempName, res->name, DEVICE_ID_MAX_LENGTH);
                    //tempName[DEVICE_ID_MAX_LENGTH - 1] = '\0'; // 保險
                    UpdateDeviceID((uint32_t)res->srcId, tempName);
                }
                // 刪除封包
                radio.deletePacket(packet);
            }
        }
    }
}

void sendNameRequest(uint16_t targetId) {
    #if ENABLE_SLEEP
    if (xSemaphoreTake(sleepLock, portMAX_DELAY) == pdTRUE) {
    #endif
        NameReqPacket req;
        req.appId = APP_ID_NAME_REQ;
        req.targetId = targetId;
        
        DEBUG_PRINTF("[TX] Asking name for Node 0x%04X...\n", targetId);
        
        radio.createPacketAndSend(BROADCAST_ADDR, (uint8_t*)&req, sizeof(NameReqPacket));
    #if ENABLE_SLEEP
        xSemaphoreGive(sleepLock);
    }
    #endif
}

void sendNameResponse() {
    #if ENABLE_SLEEP
    if (xSemaphoreTake(sleepLock, portMAX_DELAY) == pdTRUE) {
    #endif
        NameResPacket res;
        res.appId = APP_ID_NAME_RES;
        res.srcId = (uint16_t)(myDevice.GetUID() & 0xFFFF);
        // 使用 strncpy 防止溢位，保留最後一位給結束符號
        strncpy(res.name, myDevice.GetID(), DEVICE_ID_MAX_LENGTH - 1);
        res.name[DEVICE_ID_MAX_LENGTH - 1] = '\0'; // 確保結尾
        
        DEBUG_PRINTF("[TX] Replying my name: %s\n", res.name);
        
        radio.createPacketAndSend(BROADCAST_ADDR, (uint8_t*)&res, sizeof(NameResPacket));
    #if ENABLE_SLEEP
        xSemaphoreGive(sleepLock);
    }
    #endif
}

// 發送任務
void TaskLoRaSender(void *pvParameters) {
    int32_t nextInterval;
    int32_t remainingTime;

    for (;;) 
    {
        NodeData payload;
        payload.appId = MOUNTAIN_APP_ID;
        payload.nodeId = myDevice.GetUID();
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

        #if ENABLE_SLEEP
        //檢查封包是否發完
        while (radio.getSendQueueSize() > 0 || digitalRead(LORA_BUSY) == HIGH) {
            //DEBUG_PRINT("LoRa is busy processing... waiting.");
            vTaskDelay(50 / portTICK_PERIOD_MS); 
        }
        //vTaskDelay(500 / portTICK_PERIOD_MS);   //讓封包有時間發完
        #endif
        
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
        
        #if ENABLE_SLEEP
        //Light Sleep
        remainingTime = nextInterval;
        while (remainingTime > 0) {
            uint32_t startWaitTime = millis(); // 記錄開始時間


            if (xSemaphoreTake(sleepLock, 0) != pdTRUE) {
                vTaskDelay(100 / portTICK_PERIOD_MS);
                uint32_t passed = millis() - startWaitTime;
                if (passed >= remainingTime) remainingTime = 0;
                else remainingTime -= passed;
                continue; 
            }
            //DEBUG_PRINTLN("[LoRa] Got Lock! (Holding...)");
            vTaskDelay(100 / portTICK_PERIOD_MS);

            uint32_t passedBuffer = millis() - startWaitTime;
            if (passedBuffer >= remainingTime) {
                remainingTime = 0;
            } else {
                remainingTime -= passedBuffer;
            }

            // 設定剩餘時間
            if(remainingTime <= 0){
                //DEBUG_PRINTLN("[LoRa] Giving Lock back...");
                xSemaphoreGive(sleepLock);
                break;
            }
            uint32_t sleepStart = millis();
            esp_sleep_enable_timer_wakeup(remainingTime * 1000);

            /*
            //睡前鎖定所有 SPI 輸出腳位
            digitalWrite(LORA_CS, HIGH);
            gpio_hold_en((gpio_num_t)LORA_CS);
            digitalWrite(LORA_SCK, LOW); 
            gpio_hold_en((gpio_num_t)SCK);
            digitalWrite(LORA_MOSI, LOW); 
            gpio_hold_en((gpio_num_t)MOSI);
            digitalWrite(10, HIGH);
            gpio_hold_en((gpio_num_t)10);
            */

            // 開始淺眠
            DEBUG_PRINT("Going to Light Sleep...");
            //DEBUG_PRINTF("remainingTime:%d", remainingTime);
            DEBUG_FLUSH();  //確保 Log 印完再睡
            esp_light_sleep_start();

            /*
            //解除鎖定輸出電位
            gpio_hold_dis((gpio_num_t)LORA_CS);
            gpio_hold_dis((gpio_num_t)LORA_SCK);
            gpio_hold_dis((gpio_num_t)LORA_MOSI);
            gpio_hold_dis((gpio_num_t)10);
            */

            ///*
            //Self-Test
            DEBUG_PRINT("[Self-Test] Reading Noise Level... ");
            //vTaskDelay(100 / portTICK_PERIOD_MS);
            float noise = radio.radio->getRSSI(); 
            // 檢查結果
            if (noise == -164.0 || noise == 0.0) { 
                // 回傳極值視爲有錯誤
                DEBUG_PRINT("FAIL! (Radio is dead or SPI broken)");
            } else {
                DEBUG_PRINTF("PASS! (RSSI: %.2f dBm)\n", noise);
            }
            //*/

            //DEBUG_PRINTLN("[LoRa] Giving Lock back...");
            xSemaphoreGive(sleepLock);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            //扣除已經睡掉的時間
            uint32_t sleptTotal = millis() - sleepStart;
            
            if (sleptTotal >= remainingTime) {
                remainingTime = 0;
            } else {
                remainingTime -= sleptTotal;
            }

            // 檢查是誰叫醒
            esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

            if (cause == ESP_SLEEP_WAKEUP_GPIO) {
                // LoRa 封包
                DEBUG_PRINT("Woke up by LoRa");
                // 讓接收任務有足夠時間處理
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
            else if (cause == ESP_SLEEP_WAKEUP_TIMER) {
                DEBUG_PRINT("Woke up by Timer");
                //重啟Lora
                //radio.restartRadio();
                //vTaskDelay(1000 / portTICK_PERIOD_MS);
                //radio.start();
                break;
            }
        }
        #else
        vTaskDelay(nextInterval / portTICK_PERIOD_MS);
        #endif
    }
}
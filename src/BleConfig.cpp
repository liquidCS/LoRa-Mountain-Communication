#include "BleConfig.hpp"
// 確保引用的是 NimBLE，而不是標準 BLE
#include <NimBLEDevice.h> 

Preferences preferences;
LoraConfig sysConfig;
NimBLEServer* pServer = nullptr;
NimBLECharacteristic* pCharacteristic = nullptr;

// 重啟旗標，避免在 Callback 內直接重啟
bool shouldRestart = false;

// 讀取設定
void loadSettings() {
    // 唯讀模式
    preferences.begin("lora-config", true);

    // 產生預設隨機 ID
    char buffer[32];
    randomSeed(esp_random());
    snprintf(buffer, sizeof(buffer), "LoRaNode_%03d", (int)random(0, 1000));
    String defaultName = String(buffer);

    //如果 Flash 裡已經存過 id ，會直接回傳；如果 Flash 裡找不到 id 會回傳defaultName
    String savedName = preferences.getString("id", defaultName);
    
    //c_str()用於將 Arduino 的 String 物件轉換成 C 的字元陣列 const char*
    strncpy(sysConfig.deviceId, savedName.c_str(), DEVICE_ID_MAX_LENGTH - 1);
    sysConfig.deviceId[DEVICE_ID_MAX_LENGTH - 1] = '\0';
    preferences.end();
}

// 儲存設定
void saveSettings() {
    // 讀寫模式
    preferences.begin("lora-config", false);
    // 將 sysConfig.deviceId 的內容，寫入到 id 中
    preferences.putString("id", sysConfig.deviceId);
    preferences.end();
}

// 產生 JSON
String getConfigJson() {
    // 建立 JSON 文件物件，預留 JSON_DOCUMENT_SIZE bytes 的記憶體空間
    StaticJsonDocument<JSON_DOCUMENT_SIZE> doc;
    // 建立一個 JSON 欄位 id ，填入目前的 Device ID 
    doc["id"] = sysConfig.deviceId;
    // 序列化，把 JSON 物件轉換成純文字字串
    String output;
    serializeJson(doc, output);
    return output;
}

// BLE 回調
class MyCallbacks: public NimBLECharacteristicCallbacks {
    
    // 手機修改設定，在手機 App 按下發送時，函式被觸發
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        std::string value = pCharacteristic->getValue();
        
        //DEBUG_PRINTF("收到資料長度: %d\n", value.length());

        // 檢查資料是否有效
        if (value.length() > 0) {
            //DEBUG_PRINTF("收到內容: %s\n", value.c_str());

            // JSON 解析
            StaticJsonDocument<JSON_DOCUMENT_SIZE> doc;
            DeserializationError error = deserializeJson(doc, value.c_str());
            // 如果格式不是 JSON，就報error並退出
            if (error) {
                DEBUG_PRINT("[BLE] JSON 解析失敗");
                return;
            }
            // 提取並更新資料
            // 檢查 JSON 裡面有沒有 id 這個欄位
            if (doc.containsKey("id")) {
                const char* newId = doc["id"];
                strncpy(sysConfig.deviceId, newId, DEVICE_ID_MAX_LENGTH - 1);
                sysConfig.deviceId[DEVICE_ID_MAX_LENGTH - 1] = '\0';

                // 永久存檔，把新名字寫入 Flash
                saveSettings();
                DEBUG_PRINT("[BLE] 設定已儲存，準備重開機...");
                
                // 立起重啟旗標
                shouldRestart = true;
            }
        }
    }
    
    // 手機讀取設定，當手機想查詢資料時會被觸發
    void onRead(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        // 設定回傳值，把目前的設定打包成 JSON 字串，傳給手機
        pCharacteristic->setValue(getConfigJson().c_str());
    }
};

void initBleConfig() {
    // 從 Flash 讀取舊設定
    loadSettings();
    //DEBUG_PRINTF("目前裝置 ID: %s\n", sysConfig.deviceId);

    // 產生獨一無二的藍牙名稱
    uint64_t chipid = ESP.getEfuseMac();
    char deviceName[32];
    snprintf(deviceName, sizeof(deviceName), "LoRa_Node_%04X", (uint16_t)(chipid & 0xFFFF));
    
    // 啟動藍牙，並註冊這個名字
    NimBLEDevice::init(deviceName);
    // 建立 Server
    pServer = NimBLEDevice::createServer();
    // 建立 Service
    NimBLEService* pService = pServer->createService(SERVICE_UUID);

    
    // 建立 Characteristic
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ | // 手機可讀
        NIMBLE_PROPERTY::WRITE | // 手機可寫 (要求回應)
        // 加入 WRITE_NR (Write No Response)，這樣無論手機是用 Write Request 還是 Write Command，ESP32 都能收
        NIMBLE_PROPERTY::WRITE_NR | // 手機可寫 (不要求回應)
        NIMBLE_PROPERTY::NOTIFY //ESP32 可主動推播
    );

    // 綁定回調，當有人寫入時，通知 MyCallbacks (觸發 onWrite)
    pCharacteristic->setCallbacks(new MyCallbacks());
    // 發送初始值，把目前的設定轉成 JSON 放上去
    pCharacteristic->setValue(getConfigJson().c_str());

    // 啟動 Service
    pService->start();

    //取得發射器的控制器
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    NimBLEAdvertisementData advertisementData;
    // 告訴手機有提供這個 Service UUID ，手機 App 才能過濾正確的裝置
    advertisementData.setCompleteServices(NimBLEUUID(SERVICE_UUID));
    //設定為一般可被發現模式
    advertisementData.setFlags(BLE_HS_ADV_F_DISC_GEN);
    //準備好發射內容
    pAdvertising->setAdvertisementData(advertisementData);

    // 掃描回應封包
    //藍牙的廣播封包有容量限制，如果有人回應第一個封包，就把第二個封包傳出
    NimBLEAdvertisementData scanResponseData;
    scanResponseData.setName(deviceName);
    pAdvertising->setScanResponseData(scanResponseData);
    
    //正式啟動發射器
    pAdvertising->start();
    DEBUG_PRINTF("[BLE] Started. Name: %s\n", deviceName);
}

bool isBleConnected() {
    if (pServer != nullptr) {
        return pServer->getConnectedCount() > 0;
    }
    return false;
}

void stopBleConfig() {
    NimBLEDevice::getAdvertising()->stop();
    NimBLEDevice::deinit(true);
}

void StartBLE(){
    DEBUG_PRINT(" [BLE Config Mode] Waiting for App...");
    initBleConfig(); 
    
    //等待 BLE_CONFIG_TIMEOUT 秒
    unsigned long startTime = millis();
    const unsigned long CONFIG_TIMEOUT = BLE_CONFIG_TIMEOUT; 
    
    // 重啟旗標歸零
    shouldRestart = false;

    while (millis() - startTime < CONFIG_TIMEOUT) {
        
        // 檢查是否需要重啟
        if (shouldRestart) {
            DEBUG_PRINT("偵測到重啟旗標，系統將在 1 秒後重啟...");
            delay(1000); // 給藍牙一點時間把 ACK 送回手機
            ESP.restart();
        }
        //檢查是否有人連線
        if (isBleConnected()) {
            startTime = millis(); // 重置超時
            /*
            // 減少 Log 輸出頻率，避免洗版
            static unsigned long lastLog = 0;
            if (millis() - lastLog > 2000) {
                 DEBUG_PRINT("App Connected! Holding config mode...");
                 lastLog = millis();
            }
            */
        }
        delay(10);
    }
    
    DEBUG_PRINT("Config Timeout. Stopping BLE.");

    //讀取 Preferences
    Preferences prefs;
    prefs.begin("lora-config", true); 
    // 如果讀不到就回傳空字串
    String savedID = prefs.getString("id", "");
    prefs.end();
    // 把從 Flash 讀到的名字，設定進 myDevice
    if (savedID.length() > 0) {
        myDevice.SetID(savedID.c_str(),DEVICE_ID_MAX_LENGTH);
        DEBUG_PRINTF("[Init] Load ID from NVS: %s\n", myDevice.GetID());
    }
    stopBleConfig();
}
#include <Arduino.h>
#include <LoraMesher.h>
#include <FreeRTOS.h>
#include <task.h>
#include <nvs_flash.h>

#include "config.hpp"
#include "epaper.hpp"
#include "gps.hpp"
#include "lora.hpp"
#include "BleConfig.hpp"

#if ENABLE_SLEEP
SemaphoreHandle_t sleepLock;
#endif
void setup() {
  // Debug serial 
  #ifdef DEBUG
  Serial.begin(115200);
  #endif

  //嘗試啟動 NVS 分區
  esp_err_t err = nvs_flash_init();
  
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      //兩種可挽救的錯誤，強制清除後再次嘗試初始化
      DEBUG_PRINT("NVS 分區損毀，正在格式化...");
      ESP_ERROR_CHECK(nvs_flash_erase());
      err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err); // 確保啟動是成功的，如果 err 報錯，ESP_ERROR_CHECK 會觸發當機重啟
  DEBUG_PRINT("NVS 系統就緒，設定值安全");

  

  //Light Sleep
  #if ENABLE_SLEEP
  sleepLock = xSemaphoreCreateMutex();

    //確保鎖真的建立成功
    if (sleepLock == NULL) {
        DEBUG_PRINT("Fatal Error: Could not create sleepLock!");
        while(1);
    }
  #endif

    

  // Start BLE
  StartBLE();



  // Start GPS
  TaskGPSUpdate();

  // Start Lora
  TaskLoraStart();

  // Create Screen Task
  epaper.TaskScreenUpdate();
  
  
}


void loop() {
}

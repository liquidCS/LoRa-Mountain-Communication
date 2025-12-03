#include <Arduino.h>
#include <LoraMesher.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.hpp"
#include "epaper.hpp"
#include "gps.hpp"
#include "lora.hpp"

#if ENABLE_SLEEP
SemaphoreHandle_t sleepLock;
#endif
void setup() {
  // Debug serial 
  #ifdef DEBUG
  Serial.begin(115200);
  #endif

  //Light Sleep
  #if ENABLE_SLEEP
  sleepLock = xSemaphoreCreateMutex();

    //確保鎖真的建立成功
    if (sleepLock == NULL) {
        DEBUG_PRINT("Fatal Error: Could not create sleepLock!");
        while(1);
    }
  #endif

  // Start GPS
  TaskGPSUpdate();

  // Start Lora
  TaskLoraStart();

  // Create Screen Task
  epaper.TaskScreenUpdate();
  
  
}


void loop() {
}

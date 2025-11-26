#include <Arduino.h>
#include <LoRaMesher.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.hpp"
#include "epaper.hpp"
#include "gps.hpp"
#include "lora.hpp"



void setup() {
  // Debug serial 
  #ifdef DEBUG
  Serial.begin(115200);
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

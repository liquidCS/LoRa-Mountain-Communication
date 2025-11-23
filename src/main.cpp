#include <Arduino.h>
#include <RadioLib.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.hpp"
#include "epaper.hpp"
#include "gps.hpp"



void setup() {
  // Debug serial 
  #ifdef DEBUG
  Serial.begin(115200);
  #endif

  // Start GPS
  TaskGPSUpdate();

  // Create Screen Task
  epaper.TaskScreenUpdate();
  
  
}


void loop() {
}

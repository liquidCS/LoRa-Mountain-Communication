#include <Arduino.h>
#include <RadioLib.h>
#include <FreeRTOS.h>
#include <task.h>

#include "epaper.h"
#include "gps.h"


void setup() {
  // Debug serial 
  Serial.begin(115200);

  // Start GPS
  TaskGPSUpdate();

  // Create Screen Task
  TaskDrawScreenJob();
  
  
}


void loop() {
}

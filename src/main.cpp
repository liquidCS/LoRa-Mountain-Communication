#include <Arduino.h>
#include "epaper.h"

const int screen_update_rate = 60; // s

void setup() {
  Serial.begin(115200);

  drawFullScreen();
  // sleep(5);
  // clearDisplay();  
}

void loop() {
  // put your main code here, to run repeatedly:
}
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
 
#define RGB_PIN     48      // Onboard RGB LED pin
#define NUM_PIXELS  1       // Only one LED
 
Adafruit_NeoPixel pixel(NUM_PIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);
 
void test_rgbled_functionality() {
  pixel.begin();
  pixel.setBrightness(50); // Adjust brightness (0-255)

  for(int i=0; i<256; i++) {
    static uint16_t hue = 0; // Hue goes from 0 to 65535 for full cycle
    // Convert HSV to RGB and set LED color
    uint32_t color = pixel.gamma32(pixel.ColorHSV(hue));
    pixel.setPixelColor(0, color);
    pixel.show();
  
    hue += 256; // Smooth step (smaller = slower transition)
    delay(10);  // Adjust speed of transition
  }
}

 
#include "unity.h"
#include <Arduino.h>

// Component Tests
#include "component_test/test_epaper.cpp"

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_epaper_functionality);
  return UNITY_END();
}

void setup() {
  // Wait a before start running tests 
  delay(2000);

  runUnityTests();
}
void loop() {}

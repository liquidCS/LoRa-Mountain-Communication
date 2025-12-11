#ifndef BLE_CONFIG_HPP
#define BLE_CONFIG_HPP

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include "config.hpp"
#include "device.hpp"

// 定義 UUID
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// 設定結構 (與之前相同)
struct LoraConfig {
    char deviceId[16]; 
};

void initBleConfig();
bool isBleConnected();
void StartBLE();
void stopBleConfig();

extern LoraConfig sysConfig;

#endif
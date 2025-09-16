#include <Arduino.h>
#include "DeviceManager.h"
#include "Potentiometer.h"
#include "PhotoSensor.h"
#include "RGBLed.h"
#include "PowerStrategy.h"

constexpr uint8_t PIN_POT = A0;
constexpr uint8_t PIN_LDR = A1;
constexpr uint8_t PIN_R = 9;
constexpr uint8_t PIN_G = 10;
constexpr uint8_t PIN_B = 11;

DeviceManager manager;
Potentiometer pot(PIN_POT, 8);
PhotoSensor  ldr(PIN_LDR, 8);
RGBLed rgb(PIN_R, PIN_G, PIN_B, false);
NormalPowerStrategy normalPower;
LowPowerStrategy    lowPower;
PowerStrategy *currentStrategy = &normalPower;

unsigned long lastSensorSample = 0;
unsigned long lastStatusPrint = 0;

static void applyStrategyConstraints() {
    float b = pot.normalized();
    float maxB = currentStrategy->ledMaxBrightness();
    if(b > maxB) b = maxB;
    rgb.setBrightness(b);
}

void setup() {
    Serial.begin(115200);
    manager.add(&pot);
    manager.add(&ldr);
    manager.add(&rgb);
    manager.initAll();
    Serial.println(F("System init OK (demo_main)"));
}

void loop() {
    unsigned long now = millis();
    if(Serial.available()) {
        char c = Serial.read();
        if(c=='l') { currentStrategy=&lowPower; Serial.println(F("Switched to LowPower")); }
        else if(c=='n') { currentStrategy=&normalPower; Serial.println(F("Switched to Normal")); }
    }
    if(now - lastSensorSample >= currentStrategy->sensorIntervalMs()) {
        pot.update();
        ldr.update();
        lastSensorSample = now;
        applyStrategyConstraints();
        float light = ldr.lightLevel();
        uint8_t r = (uint8_t)((1.0f - light) * 255);
        uint8_t g = (uint8_t)(light * 128);
        uint8_t b = (uint8_t)(light * 255);
        rgb.setColor(r,g,b);
    }
    rgb.update();
    if(now - lastStatusPrint > 1000) {
        Serial.print(F("Strategy=")); Serial.print(currentStrategy->name()); Serial.print(' ');
        manager.printAll(Serial);
        lastStatusPrint = now;
    }
}

#pragma once
#include "Device.h"

class AnalogSensorBase : public Device {
public:
    explicit AnalogSensorBase(uint8_t pin, uint8_t samples = 8): pin_(pin), samples_(samples) {}
    void init() override {
        pinMode(pin_, INPUT);
    }
    void update() override {
        uint32_t sum = 0;
        for(uint8_t i=0;i<samples_;++i) sum += analogRead(pin_);
        raw_ = (uint16_t)(sum / samples_);
        lastUpdateMs_ = millis();
    }
    uint16_t raw() const { return raw_; }
    unsigned long lastUpdate() const { return lastUpdateMs_; }
protected:
    uint8_t pin_;
    uint8_t samples_;
    uint16_t raw_ = 0;
    unsigned long lastUpdateMs_ = 0;
};

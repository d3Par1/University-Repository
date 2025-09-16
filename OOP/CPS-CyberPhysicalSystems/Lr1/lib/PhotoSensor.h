#pragma once
#include "AnalogSensorBase.h"

class PhotoSensor : public AnalogSensorBase {
public:
    PhotoSensor(uint8_t pin, uint8_t samples=8): AnalogSensorBase(pin, samples) {}
    const char* name() const override { return "PhotoSensor"; }
    float lightLevel() const { return raw_ / 1023.0f; }
    void printStatus(Stream &out) override {
        out.print(F("raw=")); out.print(raw_); out.print(F(" light=")); out.print(lightLevel(), 3);
    }
};

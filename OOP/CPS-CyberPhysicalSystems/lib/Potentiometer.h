#pragma once
#include "AnalogSensorBase.h"

class Potentiometer : public AnalogSensorBase {
public:
    Potentiometer(uint8_t pin, uint8_t samples=8): AnalogSensorBase(pin, samples) {}
    const char* name() const override { return "Potentiometer"; }
    float normalized() const { return raw_ / 1023.0f; }
    void printStatus(Stream &out) override {
        out.print(F("raw=")); out.print(raw_); out.print(F(" norm=")); out.print(normalized(), 3);
    }
};

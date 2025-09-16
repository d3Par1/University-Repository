#pragma once
#include <Arduino.h>

class Device {
public:
    virtual ~Device() = default;
    virtual void init() = 0;            // hardware init
    virtual void update() = 0;          // periodic update (non-blocking)
    virtual void printStatus(Stream &out) = 0; // human readable status
    virtual const char* name() const = 0;      // device name
};

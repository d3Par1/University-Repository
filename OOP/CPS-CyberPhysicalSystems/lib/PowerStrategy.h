#pragma once
#include <Arduino.h>

class PowerStrategy {
public:
    virtual ~PowerStrategy()=default;
    virtual const char* name() const =0;
    virtual uint16_t sensorIntervalMs() const =0; // how often to sample sensors
    virtual float ledMaxBrightness() const =0;    // clamp brightness
};

class NormalPowerStrategy : public PowerStrategy {
public:
    const char* name() const override { return "Normal"; }
    uint16_t sensorIntervalMs() const override { return 50; }
    float ledMaxBrightness() const override { return 1.0f; }
};

class LowPowerStrategy : public PowerStrategy {
public:
    const char* name() const override { return "LowPower"; }
    uint16_t sensorIntervalMs() const override { return 200; }
    float ledMaxBrightness() const override { return 0.4f; }
};

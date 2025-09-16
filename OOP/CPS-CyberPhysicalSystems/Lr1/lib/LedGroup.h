#pragma once
#include <Arduino.h>
#include "Device.h"
#include "RGBLed.h"

class LightingScenario {
public:
    virtual ~LightingScenario()=default;
    virtual const char* name() const =0;
    virtual void apply(RGBLed &led, unsigned long ms)=0; // adjust target color over time
};

class PulseScenario : public LightingScenario {
public:
    const char* name() const override { return "Pulse"; }
    void apply(RGBLed &led, unsigned long ms) override {
        // simple pulsing between red and blue
        uint32_t phase = (ms/20) % 512; // sawtooth 0..511
        uint8_t t = phase < 256 ? phase : (511-phase);
        led.setColor(t, 0, 255 - t);
    }
};

class RainbowScenario : public LightingScenario {
public:
    const char* name() const override { return "Rainbow"; }
    void apply(RGBLed &led, unsigned long ms) override {
        // cycle hue (very lightweight approximate)
        uint16_t hue = (ms/10) % 360; // 0..359
        uint8_t r,g,b;
        hsvToRgb(hue, 1.0f, 1.0f, r,g,b);
        led.setColor(r,g,b);
    }
private:
    static void hsvToRgb(uint16_t h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
        float C = v * s;
        float X = C * (1 - fabs(fmod(h/60.0f, 2) - 1));
        float m = v - C;
        float rf=0,gf=0,bf=0;
        uint8_t region = h / 60;
        switch(region) {
            case 0: rf=C; gf=X; bf=0; break;
            case 1: rf=X; gf=C; bf=0; break;
            case 2: rf=0; gf=C; bf=X; break;
            case 3: rf=0; gf=X; bf=C; break;
            case 4: rf=X; gf=0; bf=C; break;
            default: rf=C; gf=0; bf=X; break;
        }
        r = (uint8_t)((rf+m)*255);
        g = (uint8_t)((gf+m)*255);
        b = (uint8_t)((bf+m)*255);
    }
};

class LedGroup : public Device {
public:
    static constexpr uint8_t MAX = 4;
    LedGroup() = default;
    const char* name() const override { return "LedGroup"; }
    void init() override {
        for(uint8_t i=0;i<count_;++i) leds_[i]->init();
    }
    void update() override {
        unsigned long ms = millis();
        if(scenario_) scenario_->apply(*leds_[0], ms); // apply to first (or broadcast later)
        for(uint8_t i=0;i<count_;++i) leds_[i]->update();
    }
    void printStatus(Stream &out) override {
        out.print(F("n=")); out.print(count_);
        if(scenario_) { out.print(F(" scen=")); out.print(scenario_->name()); }
    }
    void add(RGBLed *led) { if(count_<MAX && led) leds_[count_++]=led; }
    void setScenario(LightingScenario *s) { scenario_ = s; }
private:
    RGBLed* leds_[MAX]{};
    uint8_t count_ = 0;
    LightingScenario *scenario_ = nullptr;
};

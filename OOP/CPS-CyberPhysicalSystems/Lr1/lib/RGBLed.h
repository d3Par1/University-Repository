#pragma once
#include "Device.h"

class RGBLed : public Device {
public:
    RGBLed(uint8_t rPin, uint8_t gPin, uint8_t bPin, bool commonAnode=false)
        : rPin_(rPin), gPin_(gPin), bPin_(bPin), commonAnode_(commonAnode) {}
    void init() override {
        pinMode(rPin_, OUTPUT); pinMode(gPin_, OUTPUT); pinMode(bPin_, OUTPUT);
        apply();
    }
    const char* name() const override { return "RGBLed"; }
    void setColor(uint8_t r, uint8_t g, uint8_t b) {
        rTarget_=r; gTarget_=g; bTarget_=b; changed_=true;
    }
    void setBrightness(float b) { // 0..1
        if(b<0) b=0; if(b>1) b=1; brightness_=b; changed_=true;
    }
    void update() override {
        // simple easing toward target for smooth fade
        if(changed_ || (millis() - lastUpdateMs_) > 30) {
            auto ease = [](uint8_t current, uint8_t target){
                int diff = (int)target - (int)current;
                if(diff==0) return current;
                if(abs(diff) < 4) return target; // snap when close
                return (uint8_t)(current + diff/4); // ease
            };
            rCurrent_ = ease(rCurrent_, rTarget_);
            gCurrent_ = ease(gCurrent_, gTarget_);
            bCurrent_ = ease(bCurrent_, bTarget_);
            apply();
            if(rCurrent_==rTarget_ && gCurrent_==gTarget_ && bCurrent_==bTarget_) changed_=false;
            lastUpdateMs_ = millis();
        }
    }
    void printStatus(Stream &out) override {
        out.print(F("rgb=(")); out.print(rCurrent_); out.print(','); out.print(gCurrent_); out.print(','); out.print(bCurrent_);
        out.print(F(") br=")); out.print(brightness_, 2);
    }
private:
    void apply() {
        auto scale=[this](uint8_t v){ return (uint8_t)(v * brightness_); };
        uint8_t rr = scale(rCurrent_), gg = scale(gCurrent_), bb = scale(bCurrent_);
        if(commonAnode_) { rr = 255-rr; gg=255-gg; bb=255-bb; }
        analogWrite(rPin_, rr); analogWrite(gPin_, gg); analogWrite(bPin_, bb);
    }
    uint8_t rPin_, gPin_, bPin_;
    bool commonAnode_;
    uint8_t rCurrent_{0}, gCurrent_{0}, bCurrent_{0};
    uint8_t rTarget_{0}, gTarget_{0}, bTarget_{0};
    float brightness_{1.0f};
    bool changed_ = true;
    unsigned long lastUpdateMs_ = 0;
};

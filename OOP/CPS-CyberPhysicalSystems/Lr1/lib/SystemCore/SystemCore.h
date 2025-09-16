#pragma once
#include <Arduino.h>

namespace sys {

struct Config {
    uint8_t pinPot = A0;
    uint8_t pinPhoto = A1;
    uint8_t pinLedR = 9;
    uint8_t pinLedG = 10;
    uint8_t pinLedB = 11;
    unsigned long inactivityTimeoutMs = 15000;
    unsigned long scenarioRotateMs = 20000;
    float lightDarkThreshold = 0.25f;
    float lightBrightThreshold = 0.75f;
    uint32_t statusIntervalMs = 3000;
};

class IDevice {
public:
    virtual ~IDevice()=default;
    virtual void init()=0;
    virtual void update()=0;
    virtual void status(Stream &out)=0;
    virtual const char* name() const =0;
};

} // namespace sys

namespace sys {

class PowerStrategy {
public:
    virtual ~PowerStrategy()=default;
    virtual const char* name() const =0;
    virtual uint16_t sensorIntervalMs() const =0;
    virtual float maxBrightness() const =0;
};

class NormalStrategy : public PowerStrategy {
public:
    const char* name() const override { return "Normal"; }
    uint16_t sensorIntervalMs() const override { return 50; }
    float maxBrightness() const override { return 1.0f; }
};

class LowPowerStrategy : public PowerStrategy {
public:
    const char* name() const override { return "LowPower"; }
    uint16_t sensorIntervalMs() const override { return 250; }
    float maxBrightness() const override { return 0.5f; }
};

class AnalogSensor : public IDevice {
public:
    AnalogSensor(uint8_t pin, uint8_t samples=8): pin_(pin), samples_(samples) {}
    void init() override { pinMode(pin_, INPUT); }
    void update() override {
        uint32_t sum=0; for(uint8_t i=0;i<samples_;++i) sum += analogRead(pin_);
        raw_ = (uint16_t)(sum / samples_);
        norm_ = raw_ / 1023.0f;
        lastUpdate_ = millis();
    }
    void status(Stream &out) override { out.print(F("raw=")); out.print(raw_); out.print(F(" norm=")); out.print(norm_,3); }
    const char* name() const override { return "AnalogSensor"; }
    float norm() const { return norm_; }
    uint16_t raw() const { return raw_; }
protected:
    uint8_t pin_; uint8_t samples_; uint16_t raw_{0}; float norm_{0}; unsigned long lastUpdate_{0};
};

class Potentiometer : public AnalogSensor {
public:
    using AnalogSensor::AnalogSensor;
    const char* name() const override { return "Potentiometer"; }
};

class PhotoSensor : public AnalogSensor {
public:
    using AnalogSensor::AnalogSensor;
    const char* name() const override { return "PhotoSensor"; }
};

} // namespace sys

namespace sys {

class RGBLed : public IDevice {
public:
    RGBLed(uint8_t r, uint8_t g, uint8_t b, bool commonAnode=false): r_(r), g_(g), b_(b), commonAnode_(commonAnode) {}
    const char* name() const override { return "RGBLed"; }
    void init() override { pinMode(r_,OUTPUT); pinMode(g_,OUTPUT); pinMode(b_,OUTPUT); apply(); }
    void update() override {
        if(millis() - lastEase_ < 30) return;
        auto ease=[&](uint8_t &cur, uint8_t target){ int d=(int)target-(int)cur; if(d==0) return; if(abs(d)<4) cur=target; else cur += d/4; };
        ease(rr_, tr_); ease(gg_, tg_); ease(bb_, tb_);
        apply();
        lastEase_ = millis();
    }
    void status(Stream &out) override {
        out.print(F("rgb=(")); out.print(rr_); out.print(','); out.print(gg_); out.print(','); out.print(bb_); out.print(')');
        out.print(F(" br=")); out.print(brightness_,2);
    }
    void setColor(uint8_t r,uint8_t g,uint8_t b){ tr_=r; tg_=g; tb_=b; }
    void setBrightness(float b){ if(b<0)b=0; if(b>1)b=1; brightness_=b; }
    float brightness() const { return brightness_; }
    static void hsv(uint16_t h,float s,float v,uint8_t &r,uint8_t &g,uint8_t &b){ float C=v*s; float X=C*(1-fabs(fmod(h/60.0f,2)-1)); float m=v-C; float rf=0,gf=0,bf=0; uint8_t region=h/60; switch(region){case 0: rf=C; gf=X; break; case 1: rf=X; gf=C; break; case 2: gf=C; bf=X; break; case 3: gf=X; bf=C; break; case 4: rf=X; bf=C; break; default: rf=C; bf=X; } r=(uint8_t)((rf+m)*255); g=(uint8_t)((gf+m)*255); b=(uint8_t)((bf+m)*255); }
private:
    void apply(){ auto scale=[&](uint8_t v){ return (uint8_t)(v*brightness_); }; uint8_t r=scale(rr_), g=scale(gg_), b=scale(bb_); if(commonAnode_){ r=255-r; g=255-g; b=255-b; } analogWrite(r_,r); analogWrite(g_,g); analogWrite(b_,b); }
    uint8_t r_,g_,b_; bool commonAnode_{}; uint8_t rr_{0},gg_{0},bb_{0},tr_{0},tg_{0},tb_{0}; float brightness_{1.0f}; unsigned long lastEase_{0};
};

} // namespace sys

namespace sys {

// Lighting Scenarios
class IScenario { public: virtual ~IScenario()=default; virtual const char* name() const=0; virtual void apply(RGBLed &led, unsigned long ms)=0; };

class StaticColorScenario : public IScenario { public: StaticColorScenario(uint8_t r,uint8_t g,uint8_t b): r_(r),g_(g),b_(b){} const char* name() const override { return "Static"; } void apply(RGBLed &led,unsigned long){ led.setColor(r_,g_,b_);} private: uint8_t r_,g_,b_; };

class PulseScenario : public IScenario { public: const char* name() const override { return "Pulse"; } void apply(RGBLed &led,unsigned long ms) override { uint16_t phase=(ms/15)%512; uint8_t t= phase<256? phase: (511-phase); led.setColor(t,0,255-t);} };

class RainbowScenario : public IScenario { public: const char* name() const override { return "Rainbow"; } void apply(RGBLed &led,unsigned long ms) override { uint16_t h=(ms/10)%360; uint8_t r,g,b; RGBLed::hsv(h,1,1,r,g,b); led.setColor(r,g,b);} };

class LedGroup : public IDevice { public: static constexpr uint8_t MAX=6; const char* name() const override { return "LedGroup"; } void init() override { for(uint8_t i=0;i<count_;++i) leds_[i]->init(); } void update() override { unsigned long ms=millis(); if(scenario_ && count_>0) scenario_->apply(*leds_[0],ms); for(uint8_t i=0;i<count_;++i) leds_[i]->update(); } void status(Stream &out) override { out.print(F("n=")); out.print(count_); if(scenario_){ out.print(F(" scen=")); out.print(scenario_->name()); } } void add(RGBLed *l){ if(count_<MAX && l) leds_[count_++]=l; } void setScenario(IScenario *s){ scenario_=s; } private: RGBLed* leds_[MAX]{}; uint8_t count_{0}; IScenario *scenario_{nullptr}; };

// Energy Manager coordinates strategies & adaptation
class EnergyManager { public: EnergyManager(const Config &cfg, Potentiometer &pot, PhotoSensor &photo, RGBLed &led): cfg_(cfg), pot_(pot), photo_(photo), led_(led) {}
    void begin(){ lastActivity_=millis(); }
    void tick(){ unsigned long now=millis(); sampleSensorsIfDue(now); detectActivity(now); selectStrategy(now); adaptBrightness(); }
    void attachStrategies(PowerStrategy *normal, PowerStrategy *low){ normal_=normal; low_=low; current_=normal_; }
    PowerStrategy* current() const { return current_; }
    void status(Stream &out){ out.print(F("Strategy=")); out.print(current_->name()); out.print(' '); out.print(F("Pot=")); out.print(pot_.norm(),3); out.print(' '); out.print(F("Light=")); out.print(photo_.norm(),3); out.print(' '); out.print(F("InactiveMs=")); out.print(millis()-lastActivity_); }
private:
    void sampleSensorsIfDue(unsigned long now){ if(!current_) return; if(now - lastSensor_ >= current_->sensorIntervalMs()){ pot_.update(); photo_.update(); lastSensor_=now; }}
    void detectActivity(unsigned long){ float v=pot_.norm(); if(fabs(v-lastPot_)>0.02f){ lastActivity_=millis(); lastPot_=v; }}
    void selectStrategy(unsigned long){ if(!normal_||!low_) return; bool inactive = (millis()-lastActivity_) > cfg_.inactivityTimeoutMs; PowerStrategy *t = inactive? low_: normal_; if(t!=current_) current_=t; }
    void adaptBrightness(){ if(!current_) return; float base=pot_.norm(); float light=photo_.norm(); if(light < cfg_.lightDarkThreshold) base = min(1.0f, base+0.2f); else if(light > cfg_.lightBrightThreshold) base = max(0.0f, base-0.3f); if(base> current_->maxBrightness()) base=current_->maxBrightness(); led_.setBrightness(base); }
    const Config &cfg_; Potentiometer &pot_; PhotoSensor &photo_; RGBLed &led_; PowerStrategy *normal_{nullptr}; PowerStrategy *low_{nullptr}; PowerStrategy *current_{nullptr}; unsigned long lastSensor_{0}; unsigned long lastActivity_{0}; float lastPot_{0}; };

// Simple Serial command interface
class CommandInterface { public: CommandInterface(RGBLed &led, LedGroup &group, EnergyManager &energy): led_(led), group_(group), energy_(energy) {}
    void tick(){ while(Serial.available()){ char c=Serial.read(); if(c=='\n' || c=='\r'){ process(); buffer_.idx=0; } else if(buffer_.idx < (sizeof(buffer_.data)-1)){ buffer_.data[buffer_.idx++]=c; buffer_.data[buffer_.idx]=0; } } }
private:
    void process(){ if(strcmp(buffer_.data,"pulse")==0){ if(pulse_) group_.setScenario(pulse_); Serial.println(F("OK Pulse")); }
        else if(strcmp(buffer_.data,"rainbow")==0){ if(rainbow_) group_.setScenario(rainbow_); Serial.println(F("OK Rainbow")); }
        else if(strncmp(buffer_.data,"static",6)==0){ // static r g b
            uint16_t r,g,b; if(sscanf(buffer_.data,"static %hu %hu %hu", &r,&g,&b)==3){ if(staticScen_) delete staticScen_; staticScen_= new StaticColorScenario(r,g,b); group_.setScenario(staticScen_); Serial.println(F("OK Static")); }
        } else if(strcmp(buffer_.data,"status")==0){ energy_.status(Serial); Serial.println(); }
        else Serial.println(F("? Unknown")); }
    struct { char data[40]; uint8_t idx=0; } buffer_{};
    RGBLed &led_; LedGroup &group_; EnergyManager &energy_; StaticColorScenario *staticScen_{nullptr}; PulseScenario *pulse_{nullptr}; RainbowScenario *rainbow_{nullptr};
public: void attachScenarios(PulseScenario *p, RainbowScenario *r){ pulse_=p; rainbow_=r; } };

} // namespace sys

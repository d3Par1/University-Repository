// Refactored cohesive SystemCore-based implementation
#include <Arduino.h>
#include "SystemCore/SystemCore.h"

using namespace sys;

Config cfg; // default settings; can be changed before setup finishes

// Core components
Potentiometer pot(cfg.pinPot);
PhotoSensor  photo(cfg.pinPhoto);
RGBLed       rgb(cfg.pinLedR, cfg.pinLedG, cfg.pinLedB, false);
LedGroup     group;
NormalStrategy normalStrat; LowPowerStrategy lowStrat;
PulseScenario pulse; RainbowScenario rainbow; StaticColorScenario staticWhite(255,255,255);
EnergyManager energy(cfg, pot, photo, rgb);
CommandInterface cmd(rgb, group, energy);

// Scenario rotation
unsigned long lastScenarioMs = 0; uint8_t scenIndex=0;

void rotateScenario(unsigned long now){ if(now - lastScenarioMs >= cfg.scenarioRotateMs){ scenIndex = (scenIndex+1)%3; switch(scenIndex){ case 0: group.setScenario(&pulse); break; case 1: group.setScenario(&rainbow); break; default: group.setScenario(&staticWhite); break;} lastScenarioMs=now; } }

void periodicStatus(unsigned long now){ static unsigned long last=0; if(now - last >= cfg.statusIntervalMs){ energy.status(Serial); Serial.print(' '); Serial.print(F("ScenarioIndex=")); Serial.print(scenIndex); Serial.println(); last=now; } }

void setup(){ Serial.begin(9600); group.add(&rgb); group.setScenario(&pulse); rgb.setBrightness(0.6f); energy.attachStrategies(&normalStrat,&lowStrat); energy.begin(); cmd.attachScenarios(&pulse, &rainbow); pot.init(); photo.init(); rgb.init(); group.init(); }

void loop(){ unsigned long now=millis(); cmd.tick(); energy.tick(); group.update(); rotateScenario(now); periodicStatus(now); }

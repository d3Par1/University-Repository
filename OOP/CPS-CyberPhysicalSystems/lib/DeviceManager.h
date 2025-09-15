#pragma once
#include "Device.h"

class DeviceManager {
public:
    static constexpr size_t MAX_DEVICES = 10;
    void add(Device *device) {
        if(count_ < MAX_DEVICES && device) devices_[count_++] = device;
    }
    void initAll() {
        for(size_t i=0;i<count_;++i) devices_[i]->init();
    }
    void updateAll() {
        for(size_t i=0;i<count_;++i) devices_[i]->update();
    }
    void printAll(Stream &out) {
        for(size_t i=0;i<count_;++i) {
            out.print(F("[")); out.print(i); out.print(F("] "));
            out.print(devices_[i]->name()); out.print(F(": "));
            devices_[i]->printStatus(out);
            out.println();
        }
    }
private:
    Device* devices_[MAX_DEVICES]{};
    size_t count_ = 0;
};

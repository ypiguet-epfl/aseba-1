#pragma once
#include "ThymioManager.h"
#include "ThymioInfo.h"
#include <vector>

namespace mobsya {


class AndroidSerialDeviceProber : public AbstractDeviceProber {
    Q_OBJECT
public:
    std::vector<ThymioInfo> getThymios() override;
    ~AndroidSerialDeviceProber();
    static AndroidSerialDeviceProber* instance();

private:
    AndroidSerialDeviceProber(QObject* parent = nullptr);

public:
    // For java
    void onDeviceAvailabilityChanged();
};


}    // namespace mobsya

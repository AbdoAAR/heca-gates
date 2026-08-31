#pragma once

#include <Windows.h>
#include "../sdk/EuroscopePlugIn.h"

class GateOccupancyScreen : public EuroScopePlugIn::CRadarScreen {
public:
    void OnRefresh(HDC hDC, int Phase) override;
    void OnAsrContentToBeClosed(void) override {
        delete this;
    }
};

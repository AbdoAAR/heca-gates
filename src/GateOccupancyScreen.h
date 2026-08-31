#pragma once
#include <EuroScopePlugIn.h>

class GateOccupancyScreen : public EuroScopePlugIn::CRadarScreen {
public:
    void OnRefresh(HDC hDC, int Phase) override;
};

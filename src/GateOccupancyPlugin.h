#pragma once
#include <EuroScopePlugIn.h>
#include <string>
#include <unordered_map>
#include <vector>

struct GateState {
    bool occupied = false;
    std::string callsign;
};

class GateOccupancyPlugin : public EuroScopePlugIn::CPlugIn {
public:
    GateOccupancyPlugin();
    void UpdateOccupancy();
    GateState GetGateState(const char* id) const;
    void SetEnabled(bool enabled);
    bool Enabled() const;
    void SetShowCallsign(bool enabled);
    bool ShowCallsign() const;

    bool OnCompileCommand(const char* commandLine) override;
    EuroScopePlugIn::CRadarScreen* OnRadarScreenCreated(
        const char* displayName, bool NeedRadarContent, bool GeoReferenced,
        bool CanBeSaved, bool CanBeCreated) override;

private:
    std::unordered_map<std::string, GateState> states_;
    bool enabled_ = true;
    bool showCallsign_ = true;
};

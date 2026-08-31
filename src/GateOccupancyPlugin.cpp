#include <Windows.h>
#include "GateOccupancyPlugin.h"
#include "GateOccupancyScreen.h"
#include "HecaGateData.h"

#include <algorithm>
#include <cmath>
#include <cstring>
GateOccupancyPlugin::GateOccupancyPlugin()
    : CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE,
              "HECA Gate Occupancy",
              "1.0.0",
              "HECA Sector Project",
              "Dynamic stand occupancy overlay for HECA") {}

void GateOccupancyPlugin::UpdateOccupancy() {
    states_.clear();

    for (EuroScopePlugIn::CFlightPlan fp = FlightPlanSelectFirst(); fp.IsValid(); fp = FlightPlanSelectNext(fp)) {
        EuroScopePlugIn::CRadarTarget rt = fp.GetCorrelatedRadarTarget();
        if (!rt.IsValid()) continue;

        EuroScopePlugIn::CRadarTargetPositionData posData = rt.GetPosition();
        if (!posData.IsValid()) continue;
        if (posData.GetPressureAltitude() > 3000) continue;
        if (rt.GetGS() > 35) continue; // don't mark taxiing traffic as parked

        EuroScopePlugIn::CPosition pos = posData.GetPosition();
        const char* callsign = fp.GetCallsign();

        double bestDistance = 1e12;
        const HecaGateData* bestGate = nullptr;
        for (std::size_t i = 0; i < kHecaGateCount; ++i) {
            EuroScopePlugIn::CPosition gatePos;
            gatePos.m_Latitude = kHecaGates[i].lat;
            gatePos.m_Longitude = kHecaGates[i].lon;
            double d = 6371000.0 * 2.0 * std::atan2(
                std::sqrt(std::pow(std::sin((pos.m_Latitude-gatePos.m_Latitude)*3.14159265358979323846/360.0),2) +
                          std::cos(pos.m_Latitude*3.14159265358979323846/180.0)*std::cos(gatePos.m_Latitude*3.14159265358979323846/180.0)*
                          std::pow(std::sin((pos.m_Longitude-gatePos.m_Longitude)*3.14159265358979323846/360.0),2)),
                std::sqrt(1.0-(std::pow(std::sin((pos.m_Latitude-gatePos.m_Latitude)*3.14159265358979323846/360.0),2) +
                          std::cos(pos.m_Latitude*3.14159265358979323846/180.0)*std::cos(gatePos.m_Latitude*3.14159265358979323846/180.0)*
                          std::pow(std::sin((pos.m_Longitude-gatePos.m_Longitude)*3.14159265358979323846/360.0),2))));
            if (d <= kHecaGates[i].radiusM && d < bestDistance) {
                bestDistance = d;
                bestGate = &kHecaGates[i];
            }
        }
        if (bestGate) states_[bestGate->id] = {true, callsign ? callsign : ""};
    }
}

GateState GateOccupancyPlugin::GetGateState(const char* id) const {
    auto it = states_.find(id);
    return it == states_.end() ? GateState{} : it->second;
}

void GateOccupancyPlugin::SetEnabled(bool enabled) { enabled_ = enabled; }
bool GateOccupancyPlugin::Enabled() const { return enabled_; }
void GateOccupancyPlugin::SetShowCallsign(bool enabled) { showCallsign_ = enabled; }
bool GateOccupancyPlugin::ShowCallsign() const { return showCallsign_; }

bool GateOccupancyPlugin::OnCompileCommand(const char* commandLine) {
    if (!commandLine) return false;
    if (_stricmp(commandLine, ".HECAGATES ON") == 0) { enabled_ = true; return true; }
    if (_stricmp(commandLine, ".HECAGATES OFF") == 0) { enabled_ = false; return true; }
    if (_stricmp(commandLine, ".HECAGATES CALLSIGNS ON") == 0) { showCallsign_ = true; return true; }
    if (_stricmp(commandLine, ".HECAGATES CALLSIGNS OFF") == 0) { showCallsign_ = false; return true; }
    return false;
}

EuroScopePlugIn::CRadarScreen* GateOccupancyPlugin::OnRadarScreenCreated(
    const char* displayName, bool, bool, bool, bool) {
    if (displayName && std::strcmp(displayName, "Standard ES radar screen") == 0)
        return new GateOccupancyScreen();
    return nullptr;
}

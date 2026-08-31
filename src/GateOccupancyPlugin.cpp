#include <Windows.h>

#include "GateOccupancyPlugin.h"
#include "GateOccupancyScreen.h"
#include "HecaGateData.h"

#include <algorithm>
#include <cmath>
#include <cstring>

GateOccupancyPlugin::GateOccupancyPlugin()
    : CPlugIn(
        EuroScopePlugIn::COMPATIBILITY_CODE,
        "HECA Gate Occupancy",
        "1.0.0",
        "HECA Sector Project",
        "Dynamic stand occupancy overlay for HECA")
{
}

static double DistanceMeters(
    const EuroScopePlugIn::CPosition& a,
    const EuroScopePlugIn::CPosition& b)
{
    const double pi = 3.14159265358979323846;
    const double earthRadius = 6371000.0;

    const double lat1 = a.m_Latitude * pi / 180.0;
    const double lon1 = a.m_Longitude * pi / 180.0;
    const double lat2 = b.m_Latitude * pi / 180.0;
    const double lon2 = b.m_Longitude * pi / 180.0;

    const double dLat = lat2 - lat1;
    const double dLon = lon2 - lon1;

    const double sinLat = std::sin(dLat / 2.0);
    const double sinLon = std::sin(dLon / 2.0);

    double h =
        sinLat * sinLat +
        std::cos(lat1) *
        std::cos(lat2) *
        sinLon * sinLon;

    h = std::min(1.0, std::max(0.0, h));

    return 2.0 *
        earthRadius *
        std::asin(std::sqrt(h));
}

void GateOccupancyPlugin::UpdateOccupancy()
{
    states_.clear();

    for (
        EuroScopePlugIn::CFlightPlan fp = FlightPlanSelectFirst();
        fp.IsValid();
        fp = FlightPlanSelectNext(fp)
    )
    {
        EuroScopePlugIn::CRadarTarget rt =
            fp.GetCorrelatedRadarTarget();

        if (!rt.IsValid())
            continue;

        EuroScopePlugIn::CRadarTargetPositionData posData =
            rt.GetPosition();

        if (!posData.IsValid())
            continue;

        // Ignore aircraft clearly flying.
        // Keep ground and low aircraft available for detection.
        if (posData.GetPressureAltitude() > 5000)
            continue;

        EuroScopePlugIn::CPosition aircraftPosition =
            posData.GetPosition();

        const char* callsign = fp.GetCallsign();

        double bestDistance = 999999999.0;
        const HecaGateData* bestGate = nullptr;

        for (std::size_t i = 0; i < kHecaGateCount; ++i)
        {
            EuroScopePlugIn::CPosition gatePosition;

            gatePosition.m_Latitude =
                kHecaGates[i].lat;

            gatePosition.m_Longitude =
                kHecaGates[i].lon;

            const double distance =
                DistanceMeters(
                    aircraftPosition,
                    gatePosition
                );

            // Large radius for testing.
            // Change later once detection is confirmed.
            const double detectionRadius =
                std::max(
                    kHecaGates[i].radiusM,
                    150.0
                );

            if (
                distance <= detectionRadius &&
                distance < bestDistance
            )
            {
                bestDistance = distance;
                bestGate = &kHecaGates[i];
            }
        }

        if (bestGate != nullptr)
        {
            GateState state;

            state.occupied = true;

            if (callsign != nullptr)
                state.callsign = callsign;

            states_[bestGate->id] = state;
        }
    }
}

GateState GateOccupancyPlugin::GetGateState(
    const char* id
) const
{
    if (!id)
        return GateState{};

    auto it = states_.find(id);

    if (it == states_.end())
        return GateState{};

    return it->second;
}

void GateOccupancyPlugin::SetEnabled(bool enabled)
{
    enabled_ = enabled;
}

bool GateOccupancyPlugin::Enabled() const
{
    return enabled_;
}

void GateOccupancyPlugin::SetShowCallsign(bool enabled)
{
    showCallsign_ = enabled;
}

bool GateOccupancyPlugin::ShowCallsign() const
{
    return showCallsign_;
}

bool GateOccupancyPlugin::OnCompileCommand(
    const char* commandLine
)
{
    if (!commandLine)
        return false;

    if (_stricmp(commandLine, ".HECAGATES ON") == 0)
    {
        enabled_ = true;
        return true;
    }

    if (_stricmp(commandLine, ".HECAGATES OFF") == 0)
    {
        enabled_ = false;
        return true;
    }

    if (
        _stricmp(
            commandLine,
            ".HECAGATES CALLSIGNS ON"
        ) == 0
    )
    {
        showCallsign_ = true;
        return true;
    }

    if (
        _stricmp(
            commandLine,
            ".HECAGATES CALLSIGNS OFF"
        ) == 0
    )
    {
        showCallsign_ = false;
        return true;
    }

    return false;
}

EuroScopePlugIn::CRadarScreen*
GateOccupancyPlugin::OnRadarScreenCreated(
    const char* displayName,
    bool,
    bool,
    bool,
    bool
)
{
    if (
        displayName &&
        std::strcmp(
            displayName,
            "Standard ES radar screen"
        ) == 0
    )
    {
        return new GateOccupancyScreen();
    }

    return nullptr;
}

#include <Windows.h>

#include "GateOccupancyScreen.h"
#include "GateOccupancyPlugin.h"
#include "HecaGateData.h"

#include <cstring>

void GateOccupancyScreen::OnRefresh(HDC hDC, int Phase)
{
    // Avoid relying on undefined refresh phase constants.
    (void)Phase;

    auto* plugin =
        static_cast<GateOccupancyPlugin*>(GetPlugIn());

    if (!plugin)
        return;

    if (!plugin->Enabled())
        return;

    // Update aircraft/gate occupancy states
    plugin->UpdateOccupancy();

    for (std::size_t i = 0; i < kHecaGateCount; ++i)
    {
        const HecaGateData& gate = kHecaGates[i];

        // SDK requires a CPosition and returns a POINT
        EuroScopePlugIn::CPosition gatePosition;
        gatePosition.m_Latitude = gate.lat;
        gatePosition.m_Longitude = gate.lon;

        POINT point =
            ConvertCoordFromPositionToPixel(gatePosition);

        GateState state =
            plugin->GetGateState(gate.id);

        COLORREF gateColor;

        if (state.occupied)
            gateColor = RGB(255, 0, 0);
        else
            gateColor = RGB(130, 130, 130);

        // Draw occupied/free gate marker
        HBRUSH brush = CreateSolidBrush(gateColor);
        HBRUSH oldBrush =
            static_cast<HBRUSH>(SelectObject(hDC, brush));

        Ellipse(
            hDC,
            point.x - 5,
            point.y - 5,
            point.x + 5,
            point.y + 5
        );

        SelectObject(hDC, oldBrush);
        DeleteObject(brush);

        // Draw gate label
        SetBkMode(hDC, TRANSPARENT);
        SetTextColor(hDC, gateColor);

        TextOutA(
            hDC,
            point.x + 7,
            point.y - 6,
            gate.label,
            static_cast<int>(std::strlen(gate.label))
        );

        // Draw callsign beside occupied gate
        if (state.occupied &&
            plugin->ShowCallsign() &&
            !state.callsign.empty())
        {
            SetTextColor(hDC, RGB(255, 255, 255));

            TextOutA(
                hDC,
                point.x + 7,
                point.y + 8,
                state.callsign.c_str(),
                static_cast<int>(state.callsign.length())
            );
        }
    }
}

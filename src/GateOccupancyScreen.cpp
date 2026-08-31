#include <Windows.h>

#include "GateOccupancyScreen.h"
#include "GateOccupancyPlugin.h"
#include "HecaGateData.h"

#include <cstring>

void GateOccupancyScreen::OnRefresh(
    HDC hDC,
    int Phase
)
{
    (void)Phase;

    auto* plugin =
        static_cast<GateOccupancyPlugin*>(
            GetPlugIn()
        );

    if (!plugin)
        return;

    // Plugin status text
    SetBkMode(
        hDC,
        TRANSPARENT
    );

    SetTextColor(
        hDC,
        RGB(0, 255, 0)
    );

    const char* test =
        "HECA GATE PLUGIN ACTIVE";

    TextOutA(
        hDC,
        50,
        50,
        test,
        static_cast<int>(
            std::strlen(test)
        )
    );

    if (!plugin->Enabled())
        return;

    // Update occupied/free stand states
    plugin->UpdateOccupancy();

    for (
        std::size_t i = 0;
        i < kHecaGateCount;
        ++i
    )
    {
        const HecaGateData& gate =
            kHecaGates[i];

        EuroScopePlugIn::CPosition gatePosition;

        gatePosition.m_Latitude =
            gate.lat;

        gatePosition.m_Longitude =
            gate.lon;

        POINT point =
            ConvertCoordFromPositionToPixel(
                gatePosition
            );

        GateState state =
            plugin->GetGateState(
                gate.id
            );

        // RED = occupied
        // GREEN = free
        COLORREF gateColor =
            state.occupied
            ? RGB(255, 0, 0)
            : RGB(0, 255, 0);

        // Draw stand marker
        HBRUSH brush =
            CreateSolidBrush(
                gateColor
            );

        HBRUSH oldBrush =
            static_cast<HBRUSH>(
                SelectObject(
                    hDC,
                    brush
                )
            );

        Ellipse(
            hDC,
            point.x - 12,
            point.y - 12,
            point.x + 12,
            point.y + 12
        );

        SelectObject(
            hDC,
            oldBrush
        );

        DeleteObject(
            brush
        );

        // Draw gate label
        SetTextColor(
            hDC,
            gateColor
        );

        TextOutA(
            hDC,
            point.x + 14,
            point.y - 8,
            gate.label,
            static_cast<int>(
                std::strlen(
                    gate.label
                )
            )
        );

        // Optional callsign display
        if (
            state.occupied &&
            plugin->ShowCallsign() &&
            !state.callsign.empty()
        )
        {
            SetTextColor(
                hDC,
                RGB(255, 255, 255)
            );

            TextOutA(
                hDC,
                point.x + 14,
                point.y + 8,
                state.callsign.c_str(),
                static_cast<int>(
                    state.callsign.length()
                )
            );
        }
    }
}

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

    SetBkMode(
        hDC,
        TRANSPARENT
    );

    // Debug text to confirm plugin is loaded.
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
        // DARK GRAY = empty
        COLORREF gateColor =
            state.occupied
            ? RGB(220, 40, 40)
            : RGB(70, 70, 70);

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
            point.x - 10,
            point.y - 10,
            point.x + 10,
            point.y + 10
        );

        SelectObject(
            hDC,
            oldBrush
        );

        DeleteObject(
            brush
        );

        SetTextColor(
            hDC,
            RGB(255, 255, 255)
        );

        TextOutA(
            hDC,
            point.x + 12,
            point.y - 7,
            gate.label,
            static_cast<int>(
                std::strlen(
                    gate.label
                )
            )
        );

        // Show callsign on occupied stand.
        if (
            state.occupied &&
            plugin->ShowCallsign() &&
            !state.callsign.empty()
        )
        {
            SetTextColor(
                hDC,
                RGB(255, 100, 100)
            );

            TextOutA(
                hDC,
                point.x + 12,
                point.y + 7,
                state.callsign.c_str(),
                static_cast<int>(
                    state.callsign.length()
                )
            );
        }
    }
}

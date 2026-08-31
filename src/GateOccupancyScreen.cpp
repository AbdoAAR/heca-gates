#include <Windows.h>

#include "GateOccupancyScreen.h"
#include "GateOccupancyPlugin.h"
#include "HecaGateData.h"

#include <cstring>

void GateOccupancyScreen::OnRefresh(HDC hDC, int Phase)
{
    (void)Phase;

    auto* plugin =
        static_cast<GateOccupancyPlugin*>(GetPlugIn());

    if (!plugin)
        return;

    // BIG GREEN TEST TEXT
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(0, 255, 0));

    const char* test = "HECA GATE PLUGIN ACTIVE";

    TextOutA(
        hDC,
        50,
        50,
        test,
        static_cast<int>(std::strlen(test))
    );

    if (!plugin->Enabled())
        return;

    plugin->UpdateOccupancy();

    for (std::size_t i = 0; i < kHecaGateCount; ++i)
    {
        const HecaGateData& gate = kHecaGates[i];

        EuroScopePlugIn::CPosition gatePosition;
        gatePosition.m_Latitude = gate.lat;
        gatePosition.m_Longitude = gate.lon;

        POINT point =
            ConvertCoordFromPositionToPixel(gatePosition);

        GateState state =
            plugin->GetGateState(gate.id);

        // Make occupied gates VERY obvious
        COLORREF gateColor =
            state.occupied
            ? RGB(255, 0, 0)
            : RGB(0, 255, 0);

        HBRUSH brush =
            CreateSolidBrush(gateColor);

        HBRUSH oldBrush =
            static_cast<HBRUSH>(
                SelectObject(hDC, brush)
            );

        Ellipse(
            hDC,
            point.x - 12,
            point.y - 12,
            point.x + 12,
            point.y + 12
        );

        SelectObject(hDC, oldBrush);
        DeleteObject(brush);

        SetTextColor(hDC, gateColor);

        TextOutA(
            hDC,
            point.x + 14,
            point.y - 8,
            gate.label,
            static_cast<int>(
                std::strlen(gate.label)
            )
        );
    }
}

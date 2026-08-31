#include <Windows.h>

#include "GateOccupancyScreen.h"
#include "GateOccupancyPlugin.h"
#include "HecaGateData.h"

#include <string>
#include <cstring>

void GateOccupancyScreen::OnRefresh(HDC hDC, int Phase)
{
    // This SDK version does not define REFRESH_PHASE_AFTER_TAGS.
    // Keep the Phase parameter but do not filter on it.
    (void)Phase;

    auto* plugin = static_cast<GateOccupancyPlugin*>(GetPlugIn());

    if (!plugin)
        return;

    // Get the gates from the plugin
    const auto& gates = plugin->GetGates();

    for (const auto& gate : gates)
    {
        POINT point;

        // Convert geographic coordinates to screen coordinates
        if (!ConvertCoordFromPositionToPixel(
                gate.latitude,
                gate.longitude,
                &point))
        {
            continue;
        }

        bool occupied = plugin->IsGateOccupied(gate.name);

        // Occupied = red
        // Free = grey
        COLORREF gateColor;

        if (occupied)
            gateColor = RGB(255, 0, 0);
        else
            gateColor = RGB(130, 130, 130);

        // Draw gate circle
        HBRUSH brush = CreateSolidBrush(gateColor);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, brush);

        Ellipse(
            hDC,
            point.x - 6,
            point.y - 6,
            point.x + 6,
            point.y + 6
        );

        SelectObject(hDC, oldBrush);
        DeleteObject(brush);

        // Draw gate name
        SetBkMode(hDC, TRANSPARENT);
        SetTextColor(hDC, gateColor);

        TextOutA(
            hDC,
            point.x + 8,
            point.y - 6,
            gate.name.c_str(),
            (int)gate.name.length()
        );

        // Optional callsign for occupied gates
        if (plugin->ShowCallsigns() && occupied)
        {
            std::string callsign = plugin->GetGateCallsign(gate.name);

            if (!callsign.empty())
            {
                SetTextColor(hDC, RGB(255, 255, 255));

                TextOutA(
                    hDC,
                    point.x + 8,
                    point.y + 8,
                    callsign.c_str(),
                    (int)callsign.length()
                );
            }
        }
    }
}

void GateOccupancyScreen::OnAsrContentToBeClosed(void)
{
    delete this;
}

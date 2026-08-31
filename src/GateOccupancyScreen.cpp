#include <Windows.h>

#include "GateOccupancyScreen.h"
#include "GateOccupancyPlugin.h"
#include "HecaGateData.h"

#include <string>
#include <cstring>
void GateOccupancyScreen::OnRefresh(HDC hDC, int Phase) {
    if (Phase != REFRESH_PHASE_AFTER_TAGS) return;
    auto* plugin = static_cast<GateOccupancyPlugin*>(GetPlugIn());
    if (!plugin || !plugin->Enabled()) return;

    plugin->UpdateOccupancy();

    HFONT font = CreateFontA(-14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    HGDIOBJ oldFont = SelectObject(hDC, font);
    int oldBk = SetBkMode(hDC, TRANSPARENT);

    for (std::size_t i = 0; i < kHecaGateCount; ++i) {
        EuroScopePlugIn::CPosition p;
        p.m_Latitude = kHecaGates[i].lat;
        p.m_Longitude = kHecaGates[i].lon;
        POINT px = ConvertCoordFromPositionToPixel(p);
        GateState state = plugin->GetGateState(kHecaGates[i].id);

        COLORREF color = state.occupied ? RGB(255, 35, 35) : RGB(130, 130, 130);
        SetTextColor(hDC, color);

        if (state.occupied) {
            HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 35, 35));
            HBRUSH brush = CreateSolidBrush(RGB(255, 35, 35));
            HGDIOBJ oldPen = SelectObject(hDC, pen);
            HGDIOBJ oldBrush = SelectObject(hDC, brush);
            Ellipse(hDC, px.x - 4, px.y - 4, px.x + 5, px.y + 5);
            SelectObject(hDC, oldPen);
            SelectObject(hDC, oldBrush);
            DeleteObject(pen);
            DeleteObject(brush);
        }

        TextOutA(hDC, px.x + 6, px.y - 7, kHecaGates[i].label,
                 (int)std::strlen(kHecaGates[i].label));

        if (state.occupied && plugin->ShowCallsign() && !state.callsign.empty()) {
            std::string cs = " " + state.callsign;
            TextOutA(hDC, px.x + 20, px.y + 8, cs.c_str(), (int)cs.size());
        }
    }

    SetBkMode(hDC, oldBk);
    SelectObject(hDC, oldFont);
    DeleteObject(font);
}

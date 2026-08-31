# HECA Gate Occupancy

EuroScope plugin project built around the supplied EuroScope SDK files and HECA sector data.

## Function

- HECA numbered stand labels are embedded from the supplied sector package.
- Every refresh, correlated aircraft are checked against stand positions.
- Aircraft above 3000 ft or moving faster than 35 kt are ignored.
- Closest stand within 32 m is marked occupied.
- Occupied stand = red dot + red stand label.
- Free stand = grey label.
- Optional callsign beside occupied stand.

## Commands inside EuroScope

`.HECAGATES ON`
`.HECAGATES OFF`
`.HECAGATES CALLSIGNS ON`
`.HECAGATES CALLSIGNS OFF`

## Build

1. Install Visual Studio 2022 Community with **Desktop development with C++** and CMake.
2. Open this folder.
3. Run `build_release.bat`.
4. The output should be `HECA_GateOccupancy.dll`.
5. Put the DLL where you keep EuroScope plugins and load it through EuroScope's plugin loader.

## Important

This project includes the exact `EuroscopePlugIn.h` and `EuroScopePlugInDll.lib` files uploaded in this chat. The DLL must be compiled on Windows with the MSVC x86/Win32 toolchain because the provided EuroScope import library determines the architecture.

The source uses the SDK's `OnRadarScreenCreated`, `CRadarScreen::OnRefresh`, flight-plan enumeration, correlated radar target, and coordinate conversion APIs. fileciteturn1file0L1-L1

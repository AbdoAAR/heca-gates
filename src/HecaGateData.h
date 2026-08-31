#pragma once

#include <cstddef>

struct HecaGateData
{
    const char* id;
    const char* label;
    double lat;
    double lon;
    double radiusM;
};

static const HecaGateData kHecaGates[] =
{
    // KEEP ALL YOUR EXISTING 165 GATE ENTRIES HERE EXACTLY AS THEY ARE
    // Example:
    {"1", "1", 30.1172919444, 31.3820791667, 32.0},
    {"2", "2", 30.1174688889, 31.3823663889, 32.0},

    // ...
    // ALL THE REST OF YOUR EXISTING GATES
    // ...

    {"31_3", "31", 30.1310272222, 31.4103861111, 32.0},
};

static constexpr std::size_t kHecaGateCount =
    sizeof(kHecaGates) / sizeof(kHecaGates[0]);

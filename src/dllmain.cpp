#include <Windows.h>
#include "GateOccupancyPlugin.h"

static GateOccupancyPlugin* g_plugin = nullptr;

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) { return TRUE; }

extern "C" __declspec(dllexport)
void EuroScopePlugInInit(EuroScopePlugIn::CPlugIn** ppPlugInInstance) {
    g_plugin = new GateOccupancyPlugin();
    *ppPlugInInstance = g_plugin;
}

extern "C" __declspec(dllexport)
void EuroScopePlugInExit(void) {
    delete g_plugin;
    g_plugin = nullptr;
}

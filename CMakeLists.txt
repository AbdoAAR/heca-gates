cmake_minimum_required(VERSION 3.20)
project(HECA_GateOccupancy LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EUROSCOPE_SDK_ROOT "${CMAKE_SOURCE_DIR}/sdk" CACHE PATH "EuroScope SDK folder")
if(NOT EXISTS "${EUROSCOPE_SDK_ROOT}/EuroscopePlugIn.h")
  message(FATAL_ERROR "Missing EuroscopePlugIn.h in ${EUROSCOPE_SDK_ROOT}")
endif()
if(NOT EXISTS "${EUROSCOPE_SDK_ROOT}/EuroScopePlugInDll.lib")
  message(FATAL_ERROR "Missing EuroScopePlugInDll.lib in ${EUROSCOPE_SDK_ROOT}")
endif()

add_library(HECA_GateOccupancy SHARED
  src/dllmain.cpp
  src/GateOccupancyPlugin.cpp
  src/GateOccupancyScreen.cpp
)
target_include_directories(HECA_GateOccupancy PRIVATE src "${EUROSCOPE_SDK_ROOT}")
target_link_libraries(HECA_GateOccupancy PRIVATE "${EUROSCOPE_SDK_ROOT}/EuroScopePlugInDll.lib")
target_compile_definitions(HECA_GateOccupancy PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
set_target_properties(HECA_GateOccupancy PROPERTIES OUTPUT_NAME "HECA_GateOccupancy")

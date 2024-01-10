# ~~~
# Summary:      Local, non-generic plugin setup
# Copyright (c) 2020-2021 Mike Rossiter
# License:      GPLv3+
# ~~~

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.


# -------- Options ----------

set(OCPN_TEST_REPO
    "mike-rossiter/dashboard_tactics-alpha"
    CACHE STRING "Default repository for untagged builds"
)

#
#
# -------  Plugin setup --------
#
set(PKG_NAME dashboard_tactics_pi)
set(PKG_VERSION  1.8.0)
set(PKG_PRERELEASE "")  # Empty, or a tag like 'beta'

set(DISPLAY_NAME dashboard_tactics)    # Dialogs, installer artifacts, ...
set(PLUGIN_API_NAME dashboard_tactics) # As of GetCommonName() in plugin API
set(PKG_SUMMARY "Simulate ship movements")
set(PKG_DESCRIPTION [=[
Dashboard.
]=])

set(PKG_AUTHOR "Mike Rossiter")
set(PKG_IS_OPEN_SOURCE "yes")
set(PKG_HOMEPAGE https://github.com/Rasbats/dashboard_tactics_pi)
set(PKG_INFO_URL https://opencpn.org/OpenCPN/plugins/dashboard_tactics.html)

set(SRC
    src/DashboardConfig.cpp
    src/DashboardFunctions.h
    src/DashboardFunctions.cpp
    src/DashboardWindow.h
    src/DashboardWindowContainer.h
       src/dashboard_pi.h
 src/DashboardWindow.h
    src/DashboardWindow.cpp
    src/AddInstrumentDlg.h
    src/AddInstrumentDlg.cpp
    src/DashboardInstrumentContainer.h
    src/DashboardInstrumentContainer.cpp
    src/DashboardPreferencesDialog.h
    src/DashboardPreferencesDialog.cpp
    src/dashboard_pi.cpp
    src/instrument.cpp
    src/instrument.h
    src/iirfilter.h
    src/iirfilter.cpp
    src/compass.cpp
    src/compass.h
    src/dial.cpp
    src/dial.h
    src/gps.cpp
    src/gps.h
    src/rudder_angle.cpp
    src/rudder_angle.h
    src/speedometer.cpp
    src/speedometer.h
    src/wind.cpp
    src/wind.h
    src/depth.cpp
    src/depth.h
    src/clock.cpp
    src/clock.h
    src/icons.cpp
    src/icons.h
    src/wind_history.cpp
    src/wind_history.h
    src/baro_history.cpp
    src/baro_history.h
    src/Odograph.h
    src/Odograph.cpp
    src/from_ownship.cpp
    src/from_ownship.h
    src/wxJSON/jsonval.cpp
    src/wxJSON/jsonreader.cpp
    src/wxJSON/json_defs.h
    src/wxJSON/jsonreader.h
    src/wxJSON/jsonval.h
    src/wxJSON/jsonwriter.h
    src/tactics_pi.h
    src/tactics_pi.cpp
    src/TacticsConfig.cpp
    src/TacticsWindow.h
    src/TacticsWindow.cpp
    src/TacticsPreferencesDialog.h
    src/TacticsPreferencesDialog.cpp
    src/TacticsEnums.h
    src/TacticsStructs.h
    src/TacticsFunctions.h
    src/TacticsFunctions.cpp
    src/ExpSmooth.h
    src/ExpSmooth.cpp
    src/DoubleExpSmooth.h
    src/DoubleExpSmooth.cpp
    src/Polar.h
    src/Polar.cpp
    src/PerformanceSingle.h
    src/PerformanceSingle.cpp
    src/PolarPerformance.h
    src/PolarPerformance.cpp
    src/StreamInSkSingle.h
    src/StreamInSkSingle.cpp
    src/wxJSON/jsonwriter.cpp
    src/StreamoutSchema.h
    src/LineProtocol.h
    src/StreamoutSingle.h
    src/StreamoutSingle.cpp
    src/plugin_ids.h
    src/bearingcompass.h
    src/bearingcompass.cpp
    src/polarcompass.h
    src/polarcompass.cpp
    src/avg_wind.h
    src/avg_wind.cpp
    src/EngineDJG.h
    src/EngineDJG.cpp
    src/InstruJS.h
    src/InstruJS.cpp
    src/TimesTUI.h
    src/TimesTUI.cpp
    src/RaceStart.h
    src/RaceStart.cpp
    src/RaceStartGL.cpp
    src/RaceMark.h
    src/RaceMark.cpp
    src/RaceMarkGL.cpp
    src/SkData.h
    src/SkData.cpp
)

set(PKG_API_LIB api-18)  #  A dir in opencpn-libs/ e. g., api-17 or api-16

macro(late_init)
  # Perform initialization after the PACKAGE_NAME library, compilers
  # and ocpn::api is available.
endmacro ()

macro(add_plugin_libraries)
  # Add libraries required by this plugin

  add_subdirectory("${CMAKE_SOURCE_DIR}/opencpn-libs/nmea0183")
  target_link_libraries(${PACKAGE_NAME} ocpn::nmea0183)


  add_subdirectory("${CMAKE_SOURCE_DIR}/opencpn-libs/tinyxml")
  target_link_libraries(${PACKAGE_NAME} ocpn::tinyxml)

  add_subdirectory("${CMAKE_SOURCE_DIR}/opencpn-libs/wxJSON")
  target_link_libraries(${PACKAGE_NAME} ocpn::wxjson)

  add_subdirectory("${CMAKE_SOURCE_DIR}/opencpn-libs/plugin_dc")
  target_link_libraries(${PACKAGE_NAME} ocpn::plugin-dc)

  add_subdirectory("${CMAKE_SOURCE_DIR}/opencpn-libs/jsoncpp")
  target_link_libraries(${PACKAGE_NAME} ocpn::jsoncpp)

  # The wxsvg library enables SVG overall in the plugin
  add_subdirectory("${CMAKE_SOURCE_DIR}/opencpn-libs/wxsvg")
  target_link_libraries(${PACKAGE_NAME} ocpn::wxsvg)
endmacro ()

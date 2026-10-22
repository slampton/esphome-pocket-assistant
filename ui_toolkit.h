#pragma once

// ==============================================================================
// Pocket Assistant Modular C++20 UI Toolkit Umbrella Header (ui_toolkit.h)
// Includes all 5 single-responsibility domain headers in topological order:
//   1. ui_core.h        — Display geometry, .rodata LUTs, 3D math, blitter & touch
//   2. ui_ambient_va.h  — 20 FPS Docked Screensavers & 5-Profile Voice Assistant
//   3. ui_weather_sky.h — Telescopic Lunar Albedo LUTs & 20 FPS Weather Suite
//   4. ui_games.h       — 360-Deg IMU Tilt & 4-Game 20 FPS Arcade Suite
//   5. ui_tools_apps.h  — 8-Tool Suite, Clock, Music Player, TV Remote & System Hub
// ==============================================================================

#include "ui_core.h"
#include "ui_ambient_va.h"
#include "ui_weather_sky.h"
#include "ui_games.h"
#include "ui_tools_apps.h"

using namespace pocket_ui;
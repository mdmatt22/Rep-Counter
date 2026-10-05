#pragma once

#include <stdint.h>

// 1 = fake reps generated in software, 0 = real reed switch on PIN_REED.
// Normally set by the PlatformIO env (sim / reed), see platformio.ini.
#ifndef USE_SIMULATED_REPS
#define USE_SIMULATED_REPS 1
#endif

namespace cfg {

// --- CrowPanel 2.13" e-paper (from Elecrow's schematic) ---
constexpr int PIN_EPD_PWR = 7;  // must be HIGH to power the panel
constexpr int PIN_EPD_SCK = 12;
constexpr int PIN_EPD_MOSI = 11;
constexpr int PIN_EPD_CS = 14;
constexpr int PIN_EPD_DC = 13;
constexpr int PIN_EPD_RST = 10;
constexpr int PIN_EPD_BUSY = 9;

// 0 or 2 for portrait; flip if the screen is upside down on the machine.
// (The layout in RepDisplay.cpp is portrait-only.)
constexpr uint8_t DISPLAY_ROTATION = 0;

// Partial refreshes slowly leave ghosting; do a full (flashing) refresh after
// this many, but only while not mid-set.
constexpr uint16_t PARTIALS_BEFORE_FULL_REFRESH = 60;

// --- On-board buttons (active low) ---
constexpr int PIN_KEY_MENU = 2;    // reset workout
constexpr int PIN_KEY_EXIT = 1;    // end current set now
constexpr int PIN_DIAL_PRESS = 5;  // add one rep by hand (testing)

// --- Reed switch ---
// GPIO header pinout: GND, 3V3, IO41, IO40. Wire the switch between IO40 and GND.
constexpr int PIN_REED = 40;
// Ignore closures closer together than this (no one does a rep this fast).
constexpr uint32_t REED_MIN_REP_INTERVAL_MS = 600;
// Switch must have been open at least this long before a closure counts,
// so contact bounce / a magnet hovering at the edge of range doesn't add reps.
constexpr uint32_t REED_MIN_OPEN_MS = 150;

// --- Workout logic ---
// No rep for this long ends the set and starts the rest timer.
constexpr uint32_t SET_TIMEOUT_MS = 8000;

// --- Simulated reps ---
constexpr uint32_t SIM_FIRST_SET_DELAY_MS = 4000;
constexpr uint16_t SIM_REPS_MIN = 8;
constexpr uint16_t SIM_REPS_MAX = 12;
constexpr uint32_t SIM_REP_INTERVAL_MIN_MS = 2200;
constexpr uint32_t SIM_REP_INTERVAL_MAX_MS = 3600;
constexpr uint32_t SIM_REST_MIN_MS = 25000;  // keep > SET_TIMEOUT_MS
constexpr uint32_t SIM_REST_MAX_MS = 60000;

}  // namespace cfg

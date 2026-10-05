# Rep Counter

Rep counter and set/rest timer for a workout machine, shown on an e-ink display.

**Hardware:** Elecrow CrowPanel 2.13" ESP32-S3 e-paper (SSD1680, 250×122), sold on
Amazon as the "IoTeikXgo 2.13 Inch E-Ink Display with ESP32-S3". Reps come from a
reed switch and magnet on the machine. Until that's wired up, the firmware fakes reps.

## Screen

Portrait, with **REPS** on top and **TIME** below, everything centered. Flip it
upside down with `DISPLAY_ROTATION` (0 or 2) in `src/config.h`.

- **Lifting:** the first rep starts a set. TIME counts up the set time.
- **Resting:** if no rep comes for 8 s (`SET_TIMEOUT_MS`), the set ends. The label
  changes to **REST** and the timer counts rest time from the last rep. REPS keeps
  showing the set you just finished.
- The next rep starts a new set.

A small `SIM` in the corner means the reps are fake.

## Buttons

| Button | Action |
| --- | --- |
| Menu (top) | Reset the workout |
| Exit (bottom) | End the current set now |
| Dial press | Add one rep by hand (testing) |

## Build & flash

Uses [PlatformIO](https://platformio.org/): either the VS Code extension, or
`pip install platformio`.

```
pio run -e sim -t upload -t monitor     # fake reps (default)
pio run -e reed -t upload -t monitor    # real reed switch
```

The serial monitor (115200 baud) logs every rep and set.

## Wiring the reed switch

The 4-pin GPIO header is **GND, 3V3, IO41, IO40**. Connect the reed switch between
**IO40** and **GND**. The internal pull-up is used, so no resistor is needed.

Mount the switch so the magnet passes it **once per rep**, e.g. at the top of the
stroke. If the magnet sweeps past it on the way up and again on the way down,
every rep is counted twice. Debounce settings are in `src/config.h`.

## Code layout

- `src/config.h`: pins, timings, simulation settings
- `src/Workout.*`: set/rest state machine (no hardware code)
- `src/RepSensor.h`: rep source interface
  - `SimulatedRepSensor`: fake reps
  - `ReedSwitchRepSensor`: interrupt-driven reed switch
- `src/RepDisplay.*`: e-paper layout and drawing (GxEPD2 + U8g2 fonts)
- `src/main.cpp`: glue: sensor → workout → display

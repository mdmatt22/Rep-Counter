#include <Arduino.h>

#include "Button.h"
#include "RepDisplay.h"
#include "Workout.h"
#include "config.h"

#if USE_SIMULATED_REPS
#include "SimulatedRepSensor.h"
static SimulatedRepSensor sensor;
#else
#include "ReedSwitchRepSensor.h"
static ReedSwitchRepSensor sensor(cfg::PIN_REED, cfg::REED_MIN_REP_INTERVAL_MS,
                                  cfg::REED_MIN_OPEN_MS);
#endif

static Workout workout(cfg::SET_TIMEOUT_MS);
static RepDisplay display;

static Button menuButton(cfg::PIN_KEY_MENU);
static Button exitButton(cfg::PIN_KEY_EXIT);
static Button dialButton(cfg::PIN_DIAL_PRESS);

static Screen lastShown;
static bool needFullRefresh = true;  // first draw is always full
static uint16_t partialsSinceFull = 0;

static Screen makeScreen(uint32_t now) {
  Screen s;
  s.phase = workout.phase();
  s.reps = workout.reps();
  s.timerSec = (s.phase == Phase::Resting ? workout.restElapsedMs(now)
                                          : workout.setElapsedMs(now)) / 1000;
  s.simulated = USE_SIMULATED_REPS;
  return s;
}

static void addRep(uint32_t now) {
  workout.addRep(now);
  Serial.printf("rep %u (set %u, total %u)\n", workout.reps(), workout.setNumber(),
                workout.totalReps());
}

void setup() {
  Serial.begin(115200);
  Serial.printf("Rep Counter starting (%s reps)\n", USE_SIMULATED_REPS ? "simulated" : "reed switch");

  display.begin();
  sensor.begin();
  menuButton.begin();
  exitButton.begin();
  dialButton.begin();
}

void loop() {
  uint32_t now = millis();

  for (uint16_t n = sensor.takeNewReps(now); n > 0; --n) addRep(now);

  if (dialButton.pressed(now)) addRep(now);
  if (exitButton.pressed(now)) workout.endSet();
  if (menuButton.pressed(now)) {
    workout.reset();
    needFullRefresh = true;
    Serial.println("workout reset");
  }

  Phase before = workout.phase();
  workout.update(now);
  if (before == Phase::Lifting && workout.phase() == Phase::Resting) {
    Serial.printf("set %u done: %u reps\n", workout.setNumber(), workout.reps());
  }

  // Clear accumulated ghosting with a full refresh, but never mid-set: it
  // flashes for ~2 s and would hide reps as they happen.
  if (partialsSinceFull >= cfg::PARTIALS_BEFORE_FULL_REFRESH && workout.phase() != Phase::Lifting) {
    needFullRefresh = true;
  }

  Screen s = makeScreen(now);
  if (needFullRefresh || s != lastShown) {
    uint32_t t0 = millis();
    display.show(s, needFullRefresh);
    Serial.printf("[dbg] %s refresh %lu ms\n", needFullRefresh ? "FULL" : "partial", millis() - t0);
    partialsSinceFull = needFullRefresh ? 0 : partialsSinceFull + 1;
    needFullRefresh = false;
    lastShown = s;
  }

  delay(5);
}

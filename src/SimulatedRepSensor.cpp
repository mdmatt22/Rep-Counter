#include "SimulatedRepSensor.h"

#include <Arduino.h>

#include "config.h"

void SimulatedRepSensor::begin() {
  randomSeed(esp_random());
  scheduleSet(millis() + cfg::SIM_FIRST_SET_DELAY_MS);
}

uint16_t SimulatedRepSensor::takeNewReps(uint32_t now) {
  if ((int32_t)(now - nextRepMs_) < 0) return 0;

  if (--repsLeft_ == 0) {
    scheduleSet(now + random(cfg::SIM_REST_MIN_MS, cfg::SIM_REST_MAX_MS + 1));
  } else {
    nextRepMs_ = now + random(cfg::SIM_REP_INTERVAL_MIN_MS, cfg::SIM_REP_INTERVAL_MAX_MS + 1);
  }
  return 1;
}

void SimulatedRepSensor::scheduleSet(uint32_t startAt) {
  nextRepMs_ = startAt;
  repsLeft_ = random(cfg::SIM_REPS_MIN, cfg::SIM_REPS_MAX + 1);
}

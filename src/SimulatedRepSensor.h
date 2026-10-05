#pragma once

#include "RepSensor.h"

// Fakes a realistic workout: sets of SIM_REPS_MIN..MAX reps a few seconds
// apart, separated by rests long enough for the set to time out.
class SimulatedRepSensor : public RepSensor {
 public:
  void begin() override;
  uint16_t takeNewReps(uint32_t now) override;

 private:
  void scheduleSet(uint32_t startAt);

  uint32_t nextRepMs_ = 0;
  uint16_t repsLeft_ = 0;
};

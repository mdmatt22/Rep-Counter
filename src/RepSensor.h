#pragma once

#include <stdint.h>

// Source of reps. The real one reads a reed switch; the simulated one makes
// them up. main.cpp only talks to this interface.
class RepSensor {
 public:
  virtual ~RepSensor() = default;
  virtual void begin() = 0;
  // Number of reps detected since the last call.
  virtual uint16_t takeNewReps(uint32_t now) = 0;
};

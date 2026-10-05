#pragma once

#include <Arduino.h>

#include "RepSensor.h"

// One rep per reed switch closure (switch between the pin and GND, internal
// pull-up). Counted in an interrupt so reps aren't missed while the e-paper
// is busy refreshing.
class ReedSwitchRepSensor : public RepSensor {
 public:
  ReedSwitchRepSensor(uint8_t pin, uint32_t minRepIntervalMs, uint32_t minOpenMs)
      : pin_(pin), minRepIntervalMs_(minRepIntervalMs), minOpenMs_(minOpenMs) {}

  void begin() override;
  uint16_t takeNewReps(uint32_t now) override;

 private:
  static void IRAM_ATTR onChange(void* arg);

  const uint8_t pin_;
  const uint32_t minRepIntervalMs_;
  const uint32_t minOpenMs_;

  volatile uint32_t count_ = 0;
  volatile uint32_t lastRepMs_ = 0;
  volatile uint32_t lastOpenMs_ = 0;
  uint32_t taken_ = 0;
};

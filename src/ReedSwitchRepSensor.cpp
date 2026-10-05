#include "ReedSwitchRepSensor.h"

void ReedSwitchRepSensor::begin() {
  pinMode(pin_, INPUT_PULLUP);
  // Start "long open" so the very first closure counts.
  lastOpenMs_ = millis() - minOpenMs_;
  lastRepMs_ = millis() - minRepIntervalMs_;
  attachInterruptArg(digitalPinToInterrupt(pin_), onChange, this, CHANGE);
}

uint16_t ReedSwitchRepSensor::takeNewReps(uint32_t) {
  uint32_t count = count_;  // 32-bit read is atomic on the ESP32
  uint16_t fresh = count - taken_;
  taken_ = count;
  return fresh;
}

void IRAM_ATTR ReedSwitchRepSensor::onChange(void* arg) {
  auto* self = static_cast<ReedSwitchRepSensor*>(arg);
  uint32_t t = millis();
  bool closed = digitalRead(self->pin_) == LOW;

  if (!closed) {
    self->lastOpenMs_ = t;
    return;
  }
  // Bounce shows up as a closure right after an "open" edge; only count a
  // closure that follows a real open period and isn't impossibly fast.
  if (t - self->lastOpenMs_ >= self->minOpenMs_ &&
      t - self->lastRepMs_ >= self->minRepIntervalMs_) {
    self->count_ = self->count_ + 1;
    self->lastRepMs_ = t;
  }
}

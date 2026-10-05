#pragma once

#include <Arduino.h>

// Debounced active-low push button.
class Button {
 public:
  explicit Button(uint8_t pin) : pin_(pin) {}

  void begin() { pinMode(pin_, INPUT_PULLUP); }

  // True once per press.
  bool pressed(uint32_t now) {
    bool raw = digitalRead(pin_) == LOW;
    if (raw != lastRaw_) {
      lastRaw_ = raw;
      lastChangeMs_ = now;
    }
    if (now - lastChangeMs_ >= kDebounceMs && raw != stable_) {
      stable_ = raw;
      return stable_;
    }
    return false;
  }

 private:
  static constexpr uint32_t kDebounceMs = 30;
  const uint8_t pin_;
  bool lastRaw_ = false;
  bool stable_ = false;
  uint32_t lastChangeMs_ = 0;
};

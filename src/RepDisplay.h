#pragma once

#include <stdint.h>

#include "Workout.h"

// Everything that appears on screen. The timer is whole seconds, so two
// Screens compare equal until something visible actually changes.
struct Screen {
  Phase phase = Phase::Ready;
  uint16_t reps = 0;
  uint32_t timerSec = 0;  // set time while lifting, rest time while resting
  bool simulated = false;

  bool operator==(const Screen& o) const {
    return phase == o.phase && reps == o.reps && timerSec == o.timerSec &&
           simulated == o.simulated;
  }
  bool operator!=(const Screen& o) const { return !(*this == o); }
};

class RepDisplay {
 public:
  void begin();
  // full = flashing full refresh (clears ghosting); otherwise fast partial.
  void show(const Screen& s, bool full);

 private:
  void draw(const Screen& s);
};

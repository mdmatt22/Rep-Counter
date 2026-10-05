#pragma once

#include <stdint.h>

enum class Phase : uint8_t {
  Ready,    // nothing done yet
  Lifting,  // reps are coming in
  Resting,  // set ended, rest timer running
};

// Pure workout state machine: no Arduino dependencies, all times passed in as
// millis() values so it can be unit tested on a PC later.
//
// A set starts on its first rep and ends when no rep arrives for
// setTimeoutMs (or endSet() is called). Rest is timed from the last rep,
// since that's when the lifter actually stopped.
class Workout {
 public:
  explicit Workout(uint32_t setTimeoutMs) : setTimeoutMs_(setTimeoutMs) {}

  void addRep(uint32_t now);
  void endSet();
  void update(uint32_t now);
  void reset();

  Phase phase() const { return phase_; }
  uint16_t reps() const { return reps_; }  // current set, or the one just finished
  uint16_t setNumber() const { return setNumber_; }
  uint16_t totalReps() const { return totalReps_; }

  uint32_t setElapsedMs(uint32_t now) const;
  uint32_t restElapsedMs(uint32_t now) const;

 private:
  uint32_t setTimeoutMs_;
  Phase phase_ = Phase::Ready;
  uint16_t reps_ = 0;
  uint16_t setNumber_ = 0;
  uint16_t totalReps_ = 0;
  uint32_t setStartMs_ = 0;
  uint32_t lastRepMs_ = 0;
};

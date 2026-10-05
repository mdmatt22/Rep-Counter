#include "Workout.h"

void Workout::addRep(uint32_t now) {
  if (phase_ != Phase::Lifting) {
    phase_ = Phase::Lifting;
    ++setNumber_;
    reps_ = 0;
    setStartMs_ = now;
  }
  ++reps_;
  ++totalReps_;
  lastRepMs_ = now;
}

void Workout::endSet() {
  if (phase_ == Phase::Lifting) phase_ = Phase::Resting;
}

void Workout::update(uint32_t now) {
  if (phase_ == Phase::Lifting && now - lastRepMs_ >= setTimeoutMs_) endSet();
}

void Workout::reset() { *this = Workout(setTimeoutMs_); }

uint32_t Workout::setElapsedMs(uint32_t now) const {
  switch (phase_) {
    case Phase::Lifting: return now - setStartMs_;
    case Phase::Resting: return lastRepMs_ - setStartMs_;
    default: return 0;
  }
}

uint32_t Workout::restElapsedMs(uint32_t now) const {
  return phase_ == Phase::Resting ? now - lastRepMs_ : 0;
}

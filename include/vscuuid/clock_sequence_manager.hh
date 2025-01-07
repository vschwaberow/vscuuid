#pragma once

#include <mutex>

namespace vscuuid {

class ClockSequenceManager {
 public:
  ClockSequenceManager() = default;

  uint16_t GetClockSequence();

 private:
  std::mutex mutex_;
  uint16_t last_clock_seq_ = 0;
};

}

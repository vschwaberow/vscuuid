#include "vscuuid/clock_sequence_manager.hh"

namespace vscuuid {

uint16_t ClockSequenceManager::GetClockSequence() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (last_clock_seq_ == 0x3FFF) {
    last_clock_seq_ = 0;
  }
  return (last_clock_seq_++) & 0x3FFF;
}

}

#include "vscuuid/clock_sequence_manager.hh"

namespace vscuuid {

/**
 * @brief Retrieves the current clock sequence value.
 *
 * This function returns the current clock sequence value, ensuring that it is
 * within the range of 0 to 0x3FFF (16383). The clock sequence is incremented
 * with each call, and if it reaches the maximum value (0x3FFF), it wraps around
 * to 0.
 *
 * @return The current clock sequence value, masked to 14 bits.
 */
uint16_t ClockSequenceManager::GetClockSequence() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (last_clock_seq_ == 0x3FFF) {
    last_clock_seq_ = 0;
  }
  return (last_clock_seq_++) & 0x3FFF;
}

}

#include "vscuuid/uuid_generator_base.hh"

#include <iomanip>
#include <sstream>
#include <random>
#include <stdexcept>

namespace vscuuid {

/**
 * @brief Constructs a UuidGeneratorBase object.
 *
 * This constructor initializes the UuidGeneratorBase object and sets up the
 * clock sequence manager by creating a shared pointer to a new ClockSequenceManager instance.
 */
UuidGeneratorBase::UuidGeneratorBase()
    : clock_sequence_manager_(std::make_shared<ClockSequenceManager>()) {}

/**
 * @brief Generates a unique node ID.
 *
 * This function generates a unique 48-bit node ID using a random number generator.
 * It uses a random device to seed a Mersenne Twister 64-bit engine and generates
 * a uniformly distributed random number in the range [0, 0xFFFFFFFFFFFF].
 *
 * @return A 48-bit unique node ID.
 */
uint64_t UuidGeneratorBase::GenerateNodeId() {
  static std::random_device rd;
  static std::mt19937_64 gen(rd());
  static std::uniform_int_distribution<uint64_t> dist(0, 0xFFFFFFFFFFFF);
  return dist(gen);
}

/**
 * @brief Retrieves the current clock sequence.
 * 
 * This function returns the clock sequence value managed by the 
 * clock_sequence_manager_ instance. The clock sequence is used 
 * to help ensure the uniqueness of UUIDs generated in a 
 * distributed system.
 * 
 * @return uint16_t The current clock sequence.
 */
uint16_t UuidGeneratorBase::GetClockSequence() {
  return clock_sequence_manager_->GetClockSequence();
}

/**
 * @brief Formats the given components of a UUID into a standard string representation.
 *
 * This function takes the individual components of a UUID (time_low, time_mid,
 * time_hi_and_version, clock_seq, and node) and formats them into a single
 * string in the standard UUID format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx.
 *
 * @param time_low The low field of the timestamp.
 * @param time_mid The middle field of the timestamp.
 * @param time_hi_and_version The high field of the timestamp multiplexed with the version number.
 * @param clock_seq The clock sequence.
 * @param node The node field, typically representing the MAC address.
 * @return A string representing the formatted UUID.
 */
std::string UuidGeneratorBase::FormatUuid(uint64_t time_low, uint64_t time_mid,
                                          uint64_t time_hi_and_version,
                                          uint16_t clock_seq, uint64_t node) {
  std::ostringstream oss;
  oss << std::hex << std::setfill('0')
      << std::setw(8) << time_low << "-"
      << std::setw(4) << time_mid << "-"
      << std::setw(4) << time_hi_and_version << "-"
      << std::setw(4) << clock_seq << "-"
      << std::setw(12) << (node & 0xFFFFFFFFFFFF);
  return oss.str();
}

}

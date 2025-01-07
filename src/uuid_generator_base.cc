#include "vscuuid/uuid_generator_base.hh"

#include <iomanip>
#include <sstream>
#include <random>
#include <stdexcept>

namespace vscuuid {

UuidGeneratorBase::UuidGeneratorBase()
    : clock_sequence_manager_(std::make_shared<ClockSequenceManager>()) {}

uint64_t UuidGeneratorBase::GenerateNodeId() {
  static std::random_device rd;
  static std::mt19937_64 gen(rd());
  static std::uniform_int_distribution<uint64_t> dist(0, 0xFFFFFFFFFFFF);
  return dist(gen);
}

uint16_t UuidGeneratorBase::GetClockSequence() {
  return clock_sequence_manager_->GetClockSequence();
}

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

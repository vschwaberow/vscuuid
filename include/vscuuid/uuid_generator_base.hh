#pragma once

#include <string>
#include <memory>
#include "vscuuid/clock_sequence_manager.hh"

namespace vscuuid {

class UuidGeneratorBase {
 public:
  virtual ~UuidGeneratorBase() = default;
  virtual std::string Generate() = 0;

 protected:
  UuidGeneratorBase();

  uint64_t GenerateNodeId();
  uint16_t GetClockSequence();
  std::string FormatUuid(uint64_t time_low, uint64_t time_mid,
                         uint64_t time_hi_and_version, uint16_t clock_seq,
                         uint64_t node);

 private:
  std::shared_ptr<ClockSequenceManager> clock_sequence_manager_;
};

}

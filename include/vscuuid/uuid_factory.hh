#pragma once

#include "uuid_generator_base.hh"
#include "uuid_generators.hh"
#include <memory>

namespace vscuuid {

class UuidFactory {
 public:
  enum class UuidType {
    V1,
    V2,
    V3,
    V4,
    V5,
    V6,
    V7,
    V8,
  };

  static std::unique_ptr<UuidGeneratorBase> Create(UuidType type);
  static int DetectVersion(const std::string& uuid);
};

}
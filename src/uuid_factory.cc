#include "vscuuid/uuid_factory.hh"
#include <memory>
#include <stdexcept>

namespace vscuuid {

std::unique_ptr<UuidGeneratorBase> UuidFactory::Create(UuidType type) {
  switch (type) {
    case UuidType::V1:
      return std::make_unique<UuidV1Generator>();
    case UuidType::V2:
      return std::make_unique<UuidV2Generator>();
    case UuidType::V3:
      return std::make_unique<UuidV3Generator>();
    case UuidType::V4:
      return std::make_unique<UuidV4Generator>();
    case UuidType::V5:
      return std::make_unique<UuidV5Generator>();
    case UuidType::V6:
      return std::make_unique<UuidV6Generator>();
    case UuidType::V7:
      return std::make_unique<UuidV7Generator>();
    case UuidType::V8:
      return std::make_unique<UuidV8Generator>();
    default:
      throw std::invalid_argument("Unsupported UUID type");
  }
}

int UuidFactory::DetectVersion(const std::string& uuid) {
  if (uuid.length() != 36 || uuid[8] != '-' || uuid[13] != '-' || uuid[18] != '-' || uuid[23] != '-') {
    throw std::invalid_argument("Invalid UUID format");
  }

  char version_char = uuid[14];
  if (version_char >= '1' && version_char <= '8') {
    return version_char - '0';
  } else {
    throw std::invalid_argument("Unknown UUID version");
  }
}

}

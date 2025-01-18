#include "vscuuid/uuid_factory.hh"
#include <memory>
#include <stdexcept>

namespace vscuuid {

/**
 * @brief Factory method to create a UUID generator based on the specified UUID type.
 *
 * @param type The type of UUID generator to create. This should be one of the values from the UuidType enum.
 * @return std::unique_ptr<UuidGeneratorBase> A unique pointer to the created UUID generator.
 * @throws std::invalid_argument if the specified UUID type is not supported.
 */
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

/**
 * @brief Detects the version of a given UUID.
 *
 * This function checks the format of the provided UUID string and extracts the version number.
 * The UUID must be in the standard 8-4-4-4-12 format (36 characters long with hyphens at positions 8, 13, 18, and 23).
 *
 * @param uuid The UUID string to check.
 * @return The version number of the UUID (an integer between 1 and 8).
 * @throws std::invalid_argument if the UUID format is invalid or the version is unknown.
 */
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

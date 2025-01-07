#include <iostream>
#include <unordered_set>
#include <sstream>
#include <cstring>
#include "vscuuid/uuid_factory.hh"

bool debug_mode = false;

std::string ConvertToJson(const std::unordered_set<std::string>& uuid_set) {
  std::ostringstream oss;
  oss << "[\n";
  bool first = true;
  for (const auto& uuid : uuid_set) {
    if (!first) {
      oss << ",\n";
    }
    oss << "  \"" << uuid << "\"";
    first = false;
  }
  oss << "\n]";
  return oss.str();
}

void ValidateUuid(const std::string& uuid, char expected_version) {
  if (uuid.length() == 36 && uuid[8] == '-' && uuid[13] == '-' && uuid[18] == '-' && uuid[23] == '-') {
    std::cout << "[PASS] UUID format is valid." << std::endl;
  } else {
    std::cerr << "[FAIL] UUID format is invalid." << std::endl;
  }

  if (uuid[14] == expected_version) {
    std::cout << "[PASS] UUID version is correct." << std::endl;
  } else {
    std::cerr << "[FAIL] UUID version is incorrect." << std::endl;
  }
}

void GenerateBatch(vscuuid::UuidGeneratorBase& generator, char expected_version, const std::string& namespace_uuid = "", const std::string& name = "") {
  std::unordered_set<std::string> uuid_set;
  bool all_unique = true;
  for (int i = 0; i < 1000; ++i) {
    std::string batch_uuid;
    if (namespace_uuid.empty() && name.empty()) {
      batch_uuid = generator.Generate();
    } else {
      if (expected_version == '3') {
        batch_uuid = dynamic_cast<vscuuid::UuidV3Generator&>(generator).Generate(namespace_uuid, name + std::to_string(i));
      } else if (expected_version == '5') {
        batch_uuid = dynamic_cast<vscuuid::UuidV5Generator&>(generator).Generate(namespace_uuid, name + std::to_string(i));
      }
    }
    if (uuid_set.find(batch_uuid) != uuid_set.end()) {
      all_unique = false;
      std::cerr << "[FAIL] Duplicate UUID found: " << batch_uuid << std::endl;
      break;
    }
    uuid_set.insert(batch_uuid);
  }
  if (all_unique) {
    std::cout << "[PASS] All UUIDs in batch are unique." << std::endl;
  }

  if (debug_mode) {
    std::cout << "[INFO] UUID batch JSON:\n" << ConvertToJson(uuid_set) << std::endl;
  }
}

void TestUuid(vscuuid::UuidGeneratorBase& generator, char expected_version, const std::string& namespace_uuid = "", const std::string& name = "") {
  std::string uuid;
  if (namespace_uuid.empty() && name.empty()) {
    uuid = generator.Generate();
  } else {
    try {
      if (expected_version == '3') {
        uuid = dynamic_cast<vscuuid::UuidV3Generator&>(generator).Generate(namespace_uuid, name);
      } else if (expected_version == '5') {
        uuid = dynamic_cast<vscuuid::UuidV5Generator&>(generator).Generate(namespace_uuid, name);
      } else {
        throw std::invalid_argument("Unsupported UUID version for namespace and name generation");
      }
    } catch (const std::bad_cast& e) {
      std::cerr << "[ERROR] Bad cast during UUID generation: " << e.what() << std::endl;
      return;
    }
  }
  std::cout << "Generated UUID: " << uuid << std::endl;
  ValidateUuid(uuid, expected_version);
  GenerateBatch(generator, expected_version, namespace_uuid, name);
}

void TestClockSequenceManager() {
  std::cout << "[TEST] Testing ClockSequenceManager" << std::endl;
  vscuuid::ClockSequenceManager clock_manager;

  uint16_t seq1 = clock_manager.GetClockSequence();
  uint16_t seq2 = clock_manager.GetClockSequence();

  std::cout << "Sequence 1: " << seq1 << ", Sequence 2: " << seq2 << std::endl;

  if (seq2 == seq1 + 1) {
    std::cout << "[PASS] Clock sequence increments correctly." << std::endl;
  } else {
    std::cerr << "[FAIL] Clock sequence increment is incorrect." << std::endl;
  }
}

void TestUuidDetector() {
  std::cout << "[TEST] Testing UUID Detector" << std::endl;

  vscuuid::UuidV1Generator v1_generator;
  vscuuid::UuidV2Generator v2_generator;
  vscuuid::UuidV3Generator v3_generator;
  vscuuid::UuidV4Generator v4_generator;
  vscuuid::UuidV5Generator v5_generator;
  vscuuid::UuidV6Generator v6_generator;
  vscuuid::UuidV7Generator v7_generator;

  std::string uuid_v1 = v1_generator.Generate();
  std::string uuid_v2 = v2_generator.Generate();
  std::string namespace_uuid = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";
  std::string name = "example";
  std::string uuid_v3 = v3_generator.Generate(namespace_uuid, name);
  std::string uuid_v4 = v4_generator.Generate();
  std::string uuid_v5 = v5_generator.Generate(namespace_uuid, name);
  std::string uuid_v6 = v6_generator.Generate();
  std::string uuid_v7 = v7_generator.Generate();

  try {
    if (vscuuid::UuidFactory::DetectVersion(uuid_v1) == 1) {
      std::cout << "[PASS] Detected UUIDv1 correctly." << std::endl;
    } else {
      std::cerr << "[FAIL] Failed to detect UUIDv1." << std::endl;
    }

    if (vscuuid::UuidFactory::DetectVersion(uuid_v2) == 2) {
      std::cout << "[PASS] Detected UUIDv2 correctly." << std::endl;
    } else {
      std::cerr << "[FAIL] Failed to detect UUIDv2." << std::endl;
    }

    if (vscuuid::UuidFactory::DetectVersion(uuid_v3) == 3) {
      std::cout << "[PASS] Detected UUIDv3 correctly." << std::endl;
    } else {
      std::cerr << "[FAIL] Failed to detect UUIDv3." << std::endl;
    }

    if (vscuuid::UuidFactory::DetectVersion(uuid_v4) == 4) {
      std::cout << "[PASS] Detected UUIDv4 correctly." << std::endl;
    } else {
      std::cerr << "[FAIL] Failed to detect UUIDv4." << std::endl;
    }

    if (vscuuid::UuidFactory::DetectVersion(uuid_v5) == 5) {
      std::cout << "[PASS] Detected UUIDv5 correctly." << std::endl;
    } else {
      std::cerr << "[FAIL] Failed to detect UUIDv5." << std::endl;
    }

    if (vscuuid::UuidFactory::DetectVersion(uuid_v6) == 6) {
      std::cout << "[PASS] Detected UUIDv6 correctly." << std::endl;
    } else {
      std::cerr << "[FAIL] Failed to detect UUIDv6." << std::endl;
    }

    if (vscuuid::UuidFactory::DetectVersion(uuid_v7) == 7) {
      std::cout << "[PASS] Detected UUIDv7 correctly." << std::endl;
    } else {
      std::cerr << "[FAIL] Failed to detect UUIDv7." << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "[ERROR] Exception during UUID detection: " << e.what() << std::endl;
  }
}

void TestUuidV8() {
  std::cout << "[TEST] Generating UUIDv8" << std::endl;
  vscuuid::UuidV8Generator generator;
  std::array<uint8_t, 16> custom_data = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0};
  std::string uuid = generator.Generate(custom_data);
  std::cout << "Generated UUIDv8: " << uuid << std::endl;

  if (uuid.length() == 36 && uuid[8] == '-' && uuid[13] == '-' && uuid[18] == '-' && uuid[23] == '-') {
    std::cout << "[PASS] UUIDv8 format is valid." << std::endl;
  } else {
    std::cerr << "[FAIL] UUIDv8 format is invalid." << std::endl;
  }

  if (uuid[14] == '8') {
    std::cout << "[PASS] UUIDv8 version is correct." << std::endl;
  } else {
    std::cerr << "[FAIL] UUIDv8 version is incorrect." << std::endl;
  }

  std::cout << "[TEST] Generating batch of UUIDv8" << std::endl;
  std::unordered_set<std::string> uuid_set;
  bool all_unique = true;
  for (int i = 0; i < 1000; ++i) {
    custom_data[15] = static_cast<uint8_t>(i & 0xFF);
    custom_data[14] = static_cast<uint8_t>((i >> 8) & 0xFF);
    std::string batch_uuid = generator.Generate(custom_data);
    if (uuid_set.find(batch_uuid) != uuid_set.end()) {
      all_unique = false;
      std::cerr << "[FAIL] Duplicate UUIDv8 found: " << batch_uuid << std::endl;
      break;
    }
    uuid_set.insert(batch_uuid);
  }
  if (all_unique) {
    std::cout << "[PASS] All UUIDv8 in batch are unique." << std::endl;
  }

  if (debug_mode) {
    std::cout << "[INFO] UUIDv8 batch JSON:\n" << ConvertToJson(uuid_set) << std::endl;
  }
}

int main(int argc, char* argv[]) {
  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "-d") == 0 || std::strcmp(argv[i], "--debug") == 0) {
      debug_mode = true;
      break;
    }
  }

  std::cout << "[INFO] Running tests for vscuuid library." << std::endl;

  try {
    vscuuid::UuidV1Generator v1_generator;
    vscuuid::UuidV2Generator v2_generator;
    vscuuid::UuidV3Generator v3_generator;
    vscuuid::UuidV4Generator v4_generator;
    vscuuid::UuidV5Generator v5_generator;
    vscuuid::UuidV6Generator v6_generator;
    vscuuid::UuidV7Generator v7_generator;

    std::cout << "[TEST] Generating UUIDv1" << std::endl;
    TestUuid(v1_generator, '1');

    std::cout << "[TEST] Generating UUIDv2" << std::endl;
    TestUuid(v2_generator, '2');

    std::cout << "[TEST] Generating UUIDv3" << std::endl;
    TestUuid(v3_generator, '3', "6ba7b810-9dad-11d1-80b4-00c04fd430c8", "example");

    std::cout << "[TEST] Generating UUIDv4" << std::endl;
    TestUuid(v4_generator, '4');

    std::cout << "[TEST] Generating UUIDv5" << std::endl;
    TestUuid(v5_generator, '5', "6ba7b810-9dad-11d1-80b4-00c04fd430c8", "example");

    std::cout << "[TEST] Generating UUIDv6" << std::endl;
    TestUuid(v6_generator, '6');

    std::cout << "[TEST] Generating UUIDv7" << std::endl;
    TestUuid(v7_generator, '7');

    TestClockSequenceManager();
    TestUuidDetector();
    TestUuidV8();

    std::cout << "[INFO] All tests completed successfully." << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "[ERROR] Exception during tests: " << e.what() << std::endl;
  }

  return 0;
}

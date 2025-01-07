#pragma once

#include "uuid_generator_base.hh"
#include <string>
#include <string_view>

namespace vscuuid {

class UuidV1Generator : public UuidGeneratorBase {
 public:
  UuidV1Generator() = default;
  ~UuidV1Generator() override = default;

  std::string Generate() override;
};

class UuidV2Generator : public UuidGeneratorBase {
 public:
  UuidV2Generator();
  ~UuidV2Generator() override = default;

  std::string Generate() override;

 private:
  std::shared_ptr<ClockSequenceManager> clock_sequence_manager_;
  uint64_t GenerateNodeId();
};

class UuidV3Generator : public UuidGeneratorBase {
 public:
  UuidV3Generator();
  ~UuidV3Generator() override = default;

  std::string Generate(std::string_view namespace_uuid, std::string_view name);
  std::string Generate() override;

 private:
  std::string GenerateHash(std::string_view input);
};

class UuidV4Generator : public UuidGeneratorBase {
 public:
  UuidV4Generator();
  ~UuidV4Generator() override = default;

  std::string Generate() override;
};

class UuidV5Generator : public UuidGeneratorBase {
 public:
  UuidV5Generator();
  ~UuidV5Generator() override = default;

  std::string Generate(std::string_view namespace_uuid, std::string_view name);
  std::string Generate() override;
};

class UuidV6Generator : public UuidGeneratorBase {
 public:
  UuidV6Generator();
  ~UuidV6Generator() override = default;

  std::string Generate() override;
};

class UuidV7Generator : public UuidGeneratorBase {
 public:
  UuidV7Generator();
  ~UuidV7Generator() override = default;

  std::string Generate() override;
};

class UuidV8Generator : public UuidGeneratorBase {
 public:
  UuidV8Generator();
  ~UuidV8Generator() override = default;

  std::string Generate(const std::array<uint8_t, 16>& custom_data);
  std::string Generate() override;
};

}
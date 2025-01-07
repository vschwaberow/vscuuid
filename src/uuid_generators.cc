#include "vscuuid/uuid_generators.hh"
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <openssl/evp.h>
#include <stdexcept>

namespace vscuuid {

std::string UuidV1Generator::Generate() {
  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  uint64_t gregorian_offset = 122192928000000000ULL;
  uint64_t timestamp =
      std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / 100 + gregorian_offset;

  uint64_t time_low = timestamp & 0xFFFFFFFF;
  uint64_t time_mid = (timestamp >> 32) & 0xFFFF;
  uint64_t time_hi_and_version = (timestamp >> 48) & 0x0FFF;
  time_hi_and_version |= (1 << 12);

  uint16_t clock_seq = GetClockSequence() | 0x8000;
  uint64_t node = GenerateNodeId();

  return FormatUuid(time_low, time_mid, time_hi_and_version, clock_seq, node);
}


UuidV2Generator::UuidV2Generator() : clock_sequence_manager_(std::make_shared<ClockSequenceManager>()) {}

std::string UuidV2Generator::Generate() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    uint64_t gregorian_offset = 122192928000000000ULL;
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / 100 + gregorian_offset;

    uint32_t time_low = timestamp & 0xFFFFFFFF;
    uint16_t time_mid = (timestamp >> 32) & 0xFFFF;
    uint16_t time_hi_and_version = (timestamp >> 48) & 0x0FFF;
    time_hi_and_version |= (2 << 12); // Version 2

    uint16_t clock_seq = clock_sequence_manager_->GetClockSequence();
    clock_seq |= 0x8000; // Set the variant to DCE 1.1

    uint64_t node = GenerateNodeId();

    return FormatUuid(time_low, time_mid, time_hi_and_version, clock_seq, node);
}

uint64_t UuidV2Generator::GenerateNodeId() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis(0, 0xFFFFFFFFFFFF);
    return dis(gen);
}

UuidV3Generator::UuidV3Generator() {}

std::string UuidV3Generator::Generate(std::string_view namespace_uuid, std::string_view name) {
    std::string input = std::string(namespace_uuid) + std::string(name);

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (mdctx == nullptr) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }
    if (EVP_DigestInit_ex(mdctx, EVP_md5(), nullptr) != 1 ||
        EVP_DigestUpdate(mdctx, input.c_str(), input.size()) != 1 ||
        EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to compute MD5 hash");
    }
    EVP_MD_CTX_free(mdctx);

    uint32_t time_low = (hash[0] << 24) | (hash[1] << 16) | (hash[2] << 8) | hash[3];
    uint16_t time_mid = (hash[4] << 8) | hash[5];
    uint16_t time_hi_and_version = (hash[6] << 8) | hash[7];
    time_hi_and_version &= 0x0FFF;
    time_hi_and_version |= (3 << 12);

    uint16_t clock_seq = (hash[8] << 8) | hash[9];
    clock_seq &= 0x3FFF;
    clock_seq |= 0x8000;

    uint64_t node = 0;
    for (int i = 0; i < 6; ++i) {
        node |= static_cast<uint64_t>(hash[10 + i]) << (8 * (5 - i));
    }

    return FormatUuid(time_low, time_mid, time_hi_and_version, clock_seq, node);
}

std::string UuidV3Generator::Generate() {
    std::string namespace_uuid = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";
    std::string name = "default";
    return Generate(namespace_uuid, name);
}

UuidV4Generator::UuidV4Generator() {}

std::string UuidV4Generator::Generate() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    uint32_t time_low = dis(gen) & 0xFFFFFFFF;
    uint16_t time_mid = dis(gen) & 0xFFFF;
    uint16_t time_hi_and_version = (dis(gen) & 0x0FFF) | (4 << 12);

    uint16_t clock_seq = (dis(gen) & 0x3FFF) | 0x8000;

    uint64_t node = dis(gen) & 0xFFFFFFFFFFFF;

    std::ostringstream oss;
    oss << std::hex << std::setfill('0')
        << std::setw(8) << time_low << "-"
        << std::setw(4) << time_mid << "-"
        << std::setw(4) << time_hi_and_version << "-"
        << std::setw(4) << clock_seq << "-"
        << std::setw(12) << node;
    return oss.str();
}

UuidV5Generator::UuidV5Generator() {}

std::string UuidV5Generator::Generate(std::string_view namespace_uuid, std::string_view name) {
    std::string input = std::string(namespace_uuid) + std::string(name);

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (mdctx == nullptr) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }
    if (EVP_DigestInit_ex(mdctx, EVP_sha1(), nullptr) != 1 ||
        EVP_DigestUpdate(mdctx, input.c_str(), input.size()) != 1 ||
        EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to compute SHA-1 hash");
    }
    EVP_MD_CTX_free(mdctx);

    uint32_t time_low = (hash[0] << 24) | (hash[1] << 16) | (hash[2] << 8) | hash[3];
    uint16_t time_mid = (hash[4] << 8) | hash[5];
    uint16_t time_hi_and_version = (hash[6] << 8) | hash[7];
    time_hi_and_version &= 0x0FFF;
    time_hi_and_version |= (5 << 12);

    uint16_t clock_seq = (hash[8] << 8) | hash[9];
    clock_seq &= 0x3FFF;
    clock_seq |= 0x8000;

    uint64_t node = 0;
    for (int i = 0; i < 6; ++i) {
        node |= static_cast<uint64_t>(hash[10 + i]) << (8 * (5 - i));
    }

    return FormatUuid(time_low, time_mid, time_hi_and_version, clock_seq, node);
}

std::string UuidV5Generator::Generate() {
    std::string namespace_uuid = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";
    std::string name = "default";
    return Generate(namespace_uuid, name);
}

UuidV6Generator::UuidV6Generator() {}

std::string UuidV6Generator::Generate() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    uint64_t gregorian_offset = 122192928000000000ULL;
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / 100 + gregorian_offset;

    uint64_t time_low = timestamp & 0xFFFFFFFF;
    uint64_t time_mid = (timestamp >> 32) & 0xFFFF;
    uint64_t time_hi_and_version = (timestamp >> 48) & 0x0FFF;
    time_hi_and_version |= (6 << 12); // Version 6

    uint16_t clock_seq = GetClockSequence() | 0x8000;

    uint64_t node = GenerateNodeId();

    return FormatUuid(time_low, time_mid, time_hi_and_version, clock_seq, node);
}

UuidV7Generator::UuidV7Generator() {}

std::string UuidV7Generator::Generate() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    uint64_t time_high = (timestamp >> 28) & 0xFFFFFFFF;
    uint64_t time_mid = (timestamp >> 12) & 0xFFFF;
    uint64_t time_low_and_version = (timestamp & 0xFFF) | (7 << 12);

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    uint16_t clock_seq = dis(gen) & 0x3FFF;
    clock_seq |= 0x8000;

    uint64_t node = dis(gen) & 0xFFFFFFFFFFFF;

    std::ostringstream oss;
    oss << std::hex << std::setfill('0')
        << std::setw(8) << time_high << "-"
        << std::setw(4) << time_mid << "-"
        << std::setw(4) << time_low_and_version << "-"
        << std::setw(4) << clock_seq << "-"
        << std::setw(12) << node;
    return oss.str();
}


UuidV8Generator::UuidV8Generator() {}

std::string UuidV8Generator::Generate(const std::array<uint8_t, 16>& custom_data) {
    uint32_t time_low = (custom_data[0] << 24) | (custom_data[1] << 16) | (custom_data[2] << 8) | custom_data[3];
    uint16_t time_mid = (custom_data[4] << 8) | custom_data[5];
    uint16_t time_hi_and_version = (custom_data[6] << 8) | custom_data[7];
    time_hi_and_version &= 0x0FFF;
    time_hi_and_version |= (8 << 12);

    uint16_t clock_seq = (custom_data[8] << 8) | custom_data[9];
    clock_seq &= 0x3FFF;
    clock_seq |= 0x8000;

    uint64_t node = 0;
    for (int i = 0; i < 6; ++i) {
        node |= static_cast<uint64_t>(custom_data[10 + i]) << (8 * (5 - i));
    }

    std::ostringstream oss;
    oss << std::hex << std::setfill('0')
        << std::setw(8) << time_low << "-"
        << std::setw(4) << time_mid << "-"
        << std::setw(4) << time_hi_and_version << "-"
        << std::setw(4) << clock_seq << "-"
        << std::setw(12) << node;
    return oss.str();
}

std::string UuidV8Generator::Generate() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    std::array<uint8_t, 16> custom_data;
    for (auto& byte : custom_data) {
        byte = static_cast<uint8_t>(dis(gen) & 0xFF);
    }

    return Generate(custom_data);
}

}
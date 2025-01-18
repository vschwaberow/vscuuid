#include "vscuuid/uuid_generators.hh"
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <openssl/evp.h>
#include <stdexcept>

namespace vscuuid {

/**
 * @brief Generates a UUID version 1.
 * 
 * This function generates a UUID (Universally Unique Identifier) version 1,
 * which is based on the current time and the node identifier.
 * 
 * @return A string representing the generated UUID.
 */
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


/**
 * @brief Constructs a UuidV2Generator object.
 *
 * This constructor initializes the UuidV2Generator by creating a shared pointer
 * to a ClockSequenceManager instance and assigning it to the clock_sequence_manager_ member.
 */
UuidV2Generator::UuidV2Generator() : clock_sequence_manager_(std::make_shared<ClockSequenceManager>()) {}

/**
 * @brief Generates a UUID version 2.
 * 
 * This function generates a UUID (Universally Unique Identifier) version 2 based on the current system time,
 * clock sequence, and node identifier. The UUID is formatted according to the DCE 1.1 variant.
 * 
 * @return A string representing the generated UUID.
 */
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

/**
 * @brief Generates a unique node identifier.
 *
 * This function generates a unique 48-bit node identifier using a random
 * number generator. It utilizes a random device to seed a Mersenne Twister
 * 64-bit engine and produces a uniformly distributed random number in the
 * range [0, 0xFFFFFFFFFFFF].
 *
 * @return A 48-bit unique node identifier.
 */
uint64_t UuidV2Generator::GenerateNodeId() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis(0, 0xFFFFFFFFFFFF);
    return dis(gen);
}

/**
 * @brief Default constructor for the UuidV3Generator class.
 * 
 * This constructor initializes a new instance of the UuidV3Generator class.
 */
UuidV3Generator::UuidV3Generator() {}

/**
 * @brief Generates a UUID version 3 (name-based) using MD5 hashing.
 *
 * This function generates a UUID version 3 based on the provided namespace UUID
 * and name. It uses the MD5 hashing algorithm to create a hash of the combined
 * namespace UUID and name, and then formats the hash into a UUID version 3.
 *
 * @param namespace_uuid The namespace UUID as a string view.
 * @param name The name as a string view.
 * @return A string representing the generated UUID version 3.
 *
 * @throws std::runtime_error If there is an error creating the MD5 context or
 *                            computing the MD5 hash.
 */
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

/**
 * @brief Generates a UUID version 3 (name-based) using a default namespace UUID and name.
 * 
 * This function generates a UUID version 3 using a predefined namespace UUID 
 * ("6ba7b810-9dad-11d1-80b4-00c04fd430c8") and a default name ("default").
 * 
 * @return std::string The generated UUID version 3 as a string.
 */
std::string UuidV3Generator::Generate() {
    std::string namespace_uuid = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";
    std::string name = "default";
    return Generate(namespace_uuid, name);
}

/**
 * @brief Default constructor for the UuidV4Generator class.
 *
 * This constructor initializes a new instance of the UuidV4Generator class.
 */
UuidV4Generator::UuidV4Generator() {}

/**
 * @brief Generates a UUID version 4 string.
 * 
 * This function generates a random UUID (Universally Unique Identifier) 
 * version 4, which is based on random numbers.
 * 
 * @return std::string A string representation of the generated UUID v4.
 * 
 * The format of the UUID v4 string is:
 * xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
 * where 'x' is any hexadecimal digit and 'y' is one of 8, 9, A, or B.
 */
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

/**
 * @brief Default constructor for the UuidV5Generator class.
 *
 * This constructor initializes a new instance of the UuidV5Generator class.
 */
UuidV5Generator::UuidV5Generator() {}

/**
 * @brief Generates a UUID version 5 based on the given namespace UUID and name.
 *
 * This function generates a UUID version 5 by computing the SHA-1 hash of the
 * concatenation of the namespace UUID and the name. The resulting hash is then
 * used to construct the UUID according to the UUID version 5 specification.
 *
 * @param namespace_uuid The namespace UUID as a string view.
 * @param name The name as a string view.
 * @return A string representing the generated UUID version 5.
 *
 * @throws std::runtime_error If there is an error during the SHA-1 hash computation.
 */
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

/**
 * @brief Generates a UUID version 5 using a default namespace UUID and name.
 * 
 * This function generates a UUID version 5 (SHA-1 hash based) using a predefined
 * namespace UUID ("6ba7b810-9dad-11d1-80b4-00c04fd430c8") and a default name ("default").
 * 
 * @return A string representing the generated UUID version 5.
 */
std::string UuidV5Generator::Generate() {
    std::string namespace_uuid = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";
    std::string name = "default";
    return Generate(namespace_uuid, name);
}

/**
 * @brief Default constructor for the UuidV6Generator class.
 * 
 * This constructor initializes a new instance of the UuidV6Generator class.
 */
UuidV6Generator::UuidV6Generator() {}

/**
 * @brief Generates a UUID version 6.
 * 
 * This function generates a UUID (Universally Unique Identifier) version 6 based on the current system time.
 * It uses the current time since epoch, adds the Gregorian offset, and formats the timestamp according to the UUID version 6 specification.
 * 
 * @return A string representing the generated UUID version 6.
 */
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

/**
 * @brief Default constructor for the UuidV7Generator class.
 *
 * This constructor initializes a new instance of the UuidV7Generator class.
 */
UuidV7Generator::UuidV7Generator() {}

/**
 * @brief Generates a UUID version 7 string.
 * 
 * This function generates a UUID (Universally Unique Identifier) version 7
 * string based on the current system time and random values. The UUID format
 * is as follows:
 * 
 *     xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
 * 
 * where each 'x' is a hexadecimal digit.
 * 
 * @return std::string The generated UUID version 7 string.
 */
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


/**
 * @brief Default constructor for the UuidV8Generator class.
 */
UuidV8Generator::UuidV8Generator() {}

/**
 * @brief Generates a UUID version 8 string from the given custom data.
 *
 * This function takes a 16-byte array of custom data and generates a UUID version 8 string.
 * The UUID is formatted as a string in the standard 8-4-4-4-12 hexadecimal format.
 *
 * @param custom_data A 16-byte array containing the custom data used to generate the UUID.
 * @return A string representing the generated UUID version 8.
 */
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

/**
 * @brief Generates a UUID version 8 string.
 *
 * This function generates a UUID version 8 by creating a random 128-bit value
 * using a random device and a Mersenne Twister 64-bit generator. The random
 * value is then passed to another Generate function that converts it into a
 * UUID string.
 *
 * @return A string representing the generated UUID version 8.
 */
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
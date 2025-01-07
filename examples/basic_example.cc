#include <iostream>
#include <string>
#include <vector>
#include "vscuuid/uuid_factory.hh"

int main() {
    // Create UUID generators using the factory
    auto uuid_v1_generator = vscuuid::UuidFactory::Create(vscuuid::UuidFactory::UuidType::V1);
    auto uuid_v2_generator = vscuuid::UuidFactory::Create(vscuuid::UuidFactory::UuidType::V2);
    auto uuid_v3_generator = vscuuid::UuidFactory::Create(vscuuid::UuidFactory::UuidType::V3);
    auto uuid_v4_generator = vscuuid::UuidFactory::Create(vscuuid::UuidFactory::UuidType::V4);
    auto uuid_v5_generator = vscuuid::UuidFactory::Create(vscuuid::UuidFactory::UuidType::V5);
    auto uuid_v6_generator = vscuuid::UuidFactory::Create(vscuuid::UuidFactory::UuidType::V6);
    auto uuid_v7_generator = vscuuid::UuidFactory::Create(vscuuid::UuidFactory::UuidType::V7);
    auto uuid_v8_generator = vscuuid::UuidFactory::Create(vscuuid::UuidFactory::UuidType::V8);

    // Generate UUIDs
    std::string uuid_v1 = uuid_v1_generator->Generate();
    std::string uuid_v2 = uuid_v2_generator->Generate();
    std::string namespace_uuid = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";
    std::string name = "example";
    std::string uuid_v3 = dynamic_cast<vscuuid::UuidV3Generator*>(uuid_v3_generator.get())->Generate(namespace_uuid, name);
    std::string uuid_v4 = uuid_v4_generator->Generate();
    std::string uuid_v5 = dynamic_cast<vscuuid::UuidV5Generator*>(uuid_v5_generator.get())->Generate(namespace_uuid, name);
    std::string uuid_v6 = uuid_v6_generator->Generate();
    std::string uuid_v7 = uuid_v7_generator->Generate();
    std::string uuid_v8 = uuid_v8_generator->Generate();

    // Print the generated UUIDs
    std::cout << "Generated UUIDv1: " << uuid_v1 << std::endl;
    std::cout << "Generated UUIDv2: " << uuid_v2 << std::endl;
    std::cout << "Generated UUIDv3: " << uuid_v3 << std::endl;
    std::cout << "Generated UUIDv4: " << uuid_v4 << std::endl;
    std::cout << "Generated UUIDv5: " << uuid_v5 << std::endl;
    std::cout << "Generated UUIDv6: " << uuid_v6 << std::endl;
    std::cout << "Generated UUIDv7: " << uuid_v7 << std::endl;
    std::cout << "Generated UUIDv8: " << uuid_v8 << std::endl;

    // Detect UUID versions using the factory
    std::vector<std::string> uuids = {uuid_v1, uuid_v2, uuid_v3, uuid_v4, uuid_v5, uuid_v6, uuid_v7, uuid_v8};
    for (size_t i = 0; i < uuids.size(); ++i) {
        std::cout << "Detected UUID " << uuids[i] << " is version: " << vscuuid::UuidFactory::DetectVersion(uuids[i]) << std::endl;
    }

    return 0;
}
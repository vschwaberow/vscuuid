# vscuuid

`vscuuid` is a C++ library for generating and managing various versions of UUIDs (Universally Unique Identifiers). The library supports UUID versions 1, 2, 3, 4, 5, 6, 7, and 8.

## Features

- Generate UUIDs of versions 1, 2, 3, 4, 5, 6, 7, and 8.
- Detect the version of a given UUID.
- Ensure uniqueness of generated UUIDs.
- Customizable UUID generation for version 8.

## Requirements

- C++23 compiler
- CMake 3.16 or higher
- OpenSSL library

## Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/vscuuid.git
    cd vscuuid
    ```

2. Create a build directory and navigate to it:
    ```sh
    mkdir build
    cd build
    ```

3. Configure the project using CMake:
    ```sh
    cmake ..
    ```

4. Build the project:
    ```sh
    cmake --build . -- -j$(nproc)
    ```

## Usage

### Generating UUIDs

You can use the [`UuidFactory`(class to create UUID generators and generate UUIDs of different versions.

```cpp
#include <iostream>
#include "vscuuid/uuid_factory.hh"

int main() {
    auto uuid_v1_generator = vscuuid::UuidFactory::Create(vscuuid::UuidFactory::UuidType::V1);
    std::string uuid_v1 = uuid_v1_generator->Generate();
    std::cout << "Generated UUIDv1: " << uuid_v1 << std::endl;

    auto uuid_v4_generator = vscuuid::UuidFactory::Create(vscuuid::UuidFactory::UuidType::V4);
    std::string uuid_v4 = uuid_v4_generator->Generate();
    std::cout << "Generated UUIDv4: " << uuid_v4 << std::endl;

    return 0;
}
```

### Detecting UUID versions

You can use the `UuidFactory::DetectVersion`method to detect the version of the given UUID.

```cpp
#include <iostream>
#include "vscuuid/uuid_factory.hh"

int main() {
    std::string uuid = "123e4567-e89b-12d3-a456-426614174000";
    int version = vscuuid::UuidFactory::DetectVersion(uuid);
    std::cout << "Detected UUID version: " << version << std::endl;

    return 0;
}
```

## Running Tests

The project includes a set of tests to verify the functionality of the UUID generators. You can run the tests using the following commands:

1. Build the tests:

```bash
cmake --build . --target test_vscuuid
```

2. Run the tests:

```bash
./test_vscuuid
```

## Example

An example usage of the library is provided in the `examples` directory. You can build and run the example using the following commands:

1. Build the example:

```bash
cmake --build . --target basic_example
```

2. Run the example:

```bash
./basic_example
```
## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request on GitHub.

## Acknowledgements

This project uses the OpenSSL library for cryptographic functions.

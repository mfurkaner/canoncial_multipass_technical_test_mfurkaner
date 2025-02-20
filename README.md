# Ubuntu Cloud Image Fetcher

A C++ command-line tool to fetch Ubuntu Cloud image information from Simplestreams data.

## Features

- List currently supported Ubuntu Cloud releases
- Show current LTS version
- Get SHA256 hashes by:
  - Version path (e.g., "13.04/20140111")
  - Publication name (e.g., "ubuntu-trusty-14.04-amd64-server-20150227.2")
- Machine-readable clean output mode

## Build Requirements

- C++17 compatible compiler
- CMake 3.15+

## Dependencies

- nlohmann/json
- yhirose/cpp-httplib (header only)

# Build Instructions

For MacOS and Linux
```bash
mkdir build && cd build
cmake ..
cmake --build .
```


For Windows
```bash
mkdir build && cd build
cmake ..
cmake --build . --release
```
The executable will be created as 'UbuntuImageFetcher' in the build directory on Linux & MacOS, 
will be on build/Release on Windows.

# Usage

./UbuntuImageFetcher [OPTIONS]

Options:
  --help                 Show help message
  --list-releases        List supported releases
  --current-lts          Show current LTS version
  --sha256-uri <path>    Get SHA256 by version path
  --sha256-pubname <name> Get SHA256 by publication name
  --url <url>            Custom Simplestreams URL
  --clean                Machine-readable output

Default URL: https://cloud-images.ubuntu.com/releases/streams/v1/com.ubuntu.cloud:released:download.json

## Examples

List supported releases
```bash
./UbuntuImageFetcher --list-releases
```

Show current LTS
```bash
./UbuntuImageFetcher --current-lts
```

Get SHA256 by version path
```bash
./UbuntuImageFetcher --sha256-uri "13.04/20140111"
```

Get SHA256 by publication name
```bash
./UbuntuImageFetcher --sha256-pubname "ubuntu-trusty-14.04-amd64-server-20150227.2"
```

List supported releases names only
```bash
./UbuntuImageFetcher --list-releases --clean
```

Get LTS version name only
```bash
./UbuntuImageFetcher --current-lts --clean
```

Get pure SHA256 string
```bash
./UbuntuImageFetcher --sha256-uri "13.04/20140111" --clean
```
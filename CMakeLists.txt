cmake_minimum_required(VERSION 3.15)
project(UbuntuCloudVersionFetcher VERSION 1.0)

# Use C++17 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent to pull external libraries
include(FetchContent)

# Fetch the nlohmann/json library
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3 
)
FetchContent_MakeAvailable(nlohmann_json)

find_package(CURL REQUIRED)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/ubuntu_cloud_image_fetcher.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${nlohmann_json_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} 
    nlohmann_json::nlohmann_json
    CURL::libcurl 
)

include_directories(${CMAKE_SOURCE_DIR}/include)

if(APPLE)
    message(STATUS "Building for macOS")
    # macOS-specific flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building for Linux")
    # Linux-specific flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

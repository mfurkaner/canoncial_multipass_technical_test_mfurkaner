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

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/ubuntu_cloud_image_fetcher.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${nlohmann_json_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/external)

if(APPLE)
    message(STATUS "Building for macOS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building for Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif(WIN32)
    message(STATUS "Building for Windows")
    # Windows-specific compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /EHsc")
endif()
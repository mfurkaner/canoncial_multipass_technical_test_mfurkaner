cmake_minimum_required(VERSION 3.15)
project(UbuntuCloudVersionFetcher VERSION 1.0)

# Use C++17 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} 
    src/main.cpp
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

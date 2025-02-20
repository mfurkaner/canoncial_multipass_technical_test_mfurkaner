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

target_include_directories(${PROJECT_NAME} PRIVATE ${nlohmann_json_SOURCE_DIR}/include  ${CURL_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} 
    nlohmann_json::nlohmann_json
    CURL::libcurl 
)

include_directories(${CMAKE_SOURCE_DIR}/include)

if(APPLE)
    message(STATUS "Building for macOS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building for Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    target_link_libraries(${PROJECT_NAME} PRIVATE ssl crypto)
elseif(WIN32)
    message(STATUS "Building for Windows")
    # Windows-specific compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /EHsc")
    
    # Windows SDK libraries required by CURL
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        ws2_32   
        crypt32   
        Wldap32   
    )
    if(CURL_USE_STATIC_LIBS)
        add_definitions(-DCURL_STATICLIB)
    endif()
endif()

# On Windows
option(USE_OPENSSL "Use OpenSSL instead of Windows native SSL" OFF)
if(WIN32 AND USE_OPENSSL)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()
cmake_minimum_required(VERSION 3.17)
project(Data_Pipeline)

set(CMAKE_CXX_STANDARD 20)

include_directories("C:/Program Files/PostgreSQL/17/include")
link_directories("C:/Program Files/PostgreSQL/17/lib")


add_subdirectory(nlohmann_json)

add_executable(Data_Pipeline src/main.cpp
        src/LogFileReader.cpp
        src/LogLineProcessor.cpp
        src/tinyfiledialogs.c
        src/LogParser.cpp
        include/Anomaly.h
        src/ChecksAnomaly.cpp
        include/Aggregation.h
        src/Aggregation.cpp)

add_executable(Data_Pipeline_tests
        src/test.cpp
)

target_include_directories(Data_Pipeline PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/nlohmann_json
)
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

enable_testing()

target_link_libraries(Data_Pipeline PRIVATE
        nlohmann_json::nlohmann_json
        libpq
)

target_link_libraries(Data_Pipeline_tests PRIVATE
        gtest_main
        nlohmann_json::nlohmann_json
        libpq
)

include(GoogleTest)
gtest_discover_tests(Data_Pipeline_tests)

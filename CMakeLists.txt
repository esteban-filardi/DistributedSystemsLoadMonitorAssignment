cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 14)

project(DistributedSystemsLoadMonitorAssignment)

set(SOURCES
    src/main.cpp
    src/Application/Application.cpp
    src/LoadInfoReporter/LoadInfoReporter.cpp
    include/json.hpp
    src/GuidGenerator/GuidGenerator.cpp
 )

add_executable(DistributedSystemsLoadMonitorAssignment ${SOURCES})

target_link_libraries(DistributedSystemsLoadMonitorAssignment PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/libspread.a)
target_link_libraries(DistributedSystemsLoadMonitorAssignment PRIVATE -luuid)

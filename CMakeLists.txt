cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

project(DistributedSystemsLoadMonitorAssignment)

set(SOURCES
    src/main.cpp
    src/Application/Application.cpp
    src/LoadInfoReporter/LoadInfoReporter.cpp
    src/ClusterMessagesReceiver/ClusterMessagesReceiver.cpp
    src/ClusterNode/ClusterNode.cpp
    src/Cluster/Cluster.cpp
    include/json.hpp
    src/GuidGenerator/GuidGenerator.cpp
    src/AppMessageTypes/AppMessageTypes.h
    src/CpuUsageCalculator/CpuUsageCalculator.cpp
 )

add_executable(DistributedSystemsLoadMonitorAssignment ${SOURCES})

target_link_libraries(DistributedSystemsLoadMonitorAssignment PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/libspread.a)
target_link_libraries(DistributedSystemsLoadMonitorAssignment PRIVATE -luuid)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

cmake_minimum_required(VERSION 2.8)

project(netkill)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include_directories(include)

find_package(ENet REQUIRED)
include_directories(${ENET_INCLUDE_DIR})
add_definitions(${ENET_DEFINITIONS})
link_libraries(${ENET_LIBRARIES})

find_package(MsgPack REQUIRED)
include_directories(${MSGPACK_INCLUDE_DIR})
add_definitions(${MSGPACK_DEFINITIONS})
link_directories(${MSGPACK_LIB_DIR})
link_libraries(${MSGPACK_LIBRARIES})

set(client_SOURCES
    client/main.cpp)
add_executable(netkill ${client_SOURCES})

set(server_SOURCES
    server/main.cpp)
add_executable(netkilld ${server_SOURCES})

cmake_minimum_required(VERSION 3.25)
include(FetchContent)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

project(tictactoe_server)

set(CMAKE_CXX_STANDARD 23)

find_package(Qt6 COMPONENTS Widgets Network Sql REQUIRED)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(tictactoe_server src/main.cpp
        src/network/network.cpp
        src/network/network.h
        src/network/socket.h
        src/network/socket.cpp
        src/game/game_server.cpp
        src/game/game_server.h
        src/repository/database_repository.cpp
        src/repository/database_repository.h)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Network Qt6::Sql)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
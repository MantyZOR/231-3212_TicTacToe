cmake_minimum_required(VERSION 3.16)
include(FetchContent)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

project(tictactoe_client VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets Network REQUIRED)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(${PROJECT_NAME} src/main.cpp
        src/ui/renderer.cpp
        src/ui/renderer.h
        src/network/client.cpp
        src/network/client.h
        src/manager/application.cpp
        src/manager/application.h
        src/ui/components/auth_window.cpp
        src/ui/components/auth_window.h
        src/ui/components/game_list_window.cpp
        src/ui/components/game_list_window.h
        src/manager/game_coordinator.cpp
        src/manager/game_coordinator.h
        src/ui/components/game_window.cpp
        src/ui/components/game_window.h)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Network)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
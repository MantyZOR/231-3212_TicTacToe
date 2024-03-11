cmake_minimum_required(VERSION 3.25)
project(tictactoe_server)

set(CMAKE_CXX_STANDARD 23)

add_executable(tictactoe_server src/main.cpp
        src/network/network.cpp
        src/network/network.h
        src/network/socket.h
        src/network/socket.cpp
        src/game/game_server.cpp
        src/game/game_server.h)

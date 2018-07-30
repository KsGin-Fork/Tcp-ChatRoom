cmake_minimum_required(VERSION 3.10)
project(Tcp_ChatRoom_Server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

link_libraries(boost_system boost_thread)
add_executable(Tcp_ChatRoom_Server main.cpp Server.cpp Server.h)
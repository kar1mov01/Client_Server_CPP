cmake_minimum_required(VERSION 3.23)
project(Client_Server_CPP)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(Include)

file(GLOB SERVER
        Include/Network.hpp Include/Server.hpp
        Source/main_server.cpp
        )
file(GLOB CLIENT
        Include/Network.hpp Include/Client.hpp
        Source/main_client.cpp  
        )
add_executable(client ${CLIENT} )
add_executable(server ${SERVER} )

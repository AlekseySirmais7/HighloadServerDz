cmake_minimum_required(VERSION 3.10.2)
project(server)

set(CMAKE_CXX_STANDARD 17)



include_directories(headers)


#add_library(queue STATIC src/SQueue.cpp)


set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS ON)

find_package(Boost 1.57.0 COMPONENTS system filesystem thread REQUIRED)


include_directories(${Boost_INCLUDE_DIRS})


add_executable(server main.cpp headers/Server.h headers/MainHandler.h headers/Utils.h headers/FileReadersAndSenders.h headers/ThreadPool.h)

target_link_libraries(server ${Boost_LIBRARIES})
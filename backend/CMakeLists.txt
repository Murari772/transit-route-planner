cmake_minimum_required(VERSION 3.10)

project(TransitRoutePlanner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(TransitRoutePlanner ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(TransitRoutePlanner PRIVATE Threads::Threads)

if(APPLE)
    find_library(FOUNDATION_LIB Foundation)
    if(FOUNDATION_LIB)
        target_link_libraries(TransitRoutePlanner PRIVATE ${FOUNDATION_LIB})
    endif()
endif()

find_path(ASIO_INCLUDE_DIR asio.hpp
    HINTS
        /opt/homebrew/include
        /opt/homebrew/opt/asio/include
        /usr/local/include
)

if(NOT ASIO_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find standalone Asio. Install it with: brew install asio")
endif()

target_include_directories(TransitRoutePlanner PRIVATE ${ASIO_INCLUDE_DIR})

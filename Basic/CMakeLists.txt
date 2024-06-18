cmake_minimum_required(VERSION 3.28)
project(Bricks)

#set(CMAKE_CXX_FLAGS "-lncurses")
set(CMAKE_CXX_STANDARD 17)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(Bricks
        Source/main.cpp
        Source/Ball.h
        Source/Ball.cpp
        Source/Rectangle.cpp
        Source/Rectangle.h
        #        target_link_libraries(SO2-project-1 ncurses)
)

target_link_libraries(Bricks ${CURSES_LIBRARIES})

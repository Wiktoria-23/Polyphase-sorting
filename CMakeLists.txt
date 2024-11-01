cmake_minimum_required(VERSION 3.29)
project(Projekt_sortowanie)

set(CMAKE_CXX_STANDARD 20)

add_executable(Projekt_sortowanie src/main.cpp
        src/program.cpp
        src/program.h
        src/Record.cpp
        src/Record.h
        src/DiskPage.cpp
        src/DiskPage.h
        src/DataManager.cpp
        src/DataManager.h)

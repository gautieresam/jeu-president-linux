cmake_minimum_required(VERSION 3.17)
project(president_linux_processus_)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(president_linux_processus_
        client.C
        deleteMemory.c
        gestion_jeu.c
        gestion_jeu.h
        main.c
        master.c
        )

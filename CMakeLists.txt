cmake_minimum_required(VERSION 3.00)

project(fs2pg)

if(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
endif()

SET(CMAKE_C_FLAGS         "-I/usr/include/postgresql/10/server")
SET(CMAKE_C_FLAGS_RELEASE "-O3")
SET(CMAKE_C_FLAGS_DEBUG   "-O0 -g -coverage")

add_library(id2path SHARED id2path.c)

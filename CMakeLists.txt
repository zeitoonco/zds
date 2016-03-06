set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("ZDS")
include(generalSettings.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${libDir})

add_subdirectory(${libDir})
add_subdirectory(${executablesDir})
if(IS_DIRECTORY ${testsDir})
  add_subdirectory(${testsDir})
endif(IS_DIRECTORY ${testsDir})

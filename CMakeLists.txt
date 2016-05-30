set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("ZDS")
include(generalSettings.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${libDir})

execute_process(COMMAND ${PYTHON3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/resource/subProjects/CodeGenerator/main.py ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(
        autoCodeGenerator
        COMMAND ${PYTHON3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/resource/subProjects/CodeGenerator/main.py ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(${libDir})
add_dependencies(${libName} autoCodeGenerator)
add_subdirectory(${executablesDir})
add_subdirectory(${testsDir})

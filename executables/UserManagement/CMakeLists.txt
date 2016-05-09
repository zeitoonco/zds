#include source files
SET(PROJECT_SOURCE_FILES "")
INCLUDE(sources.cmake)

#extract modue name
file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} moduleName)
string(FIND ${moduleName} "/" pos1 REVERSE)
MATH(EXPR pos1 "${pos1}+1")
string(SUBSTRING ${moduleName} ${pos1} -1 moduleName)

#add executable target for module
add_executable(${moduleName} ${PROJECT_SOURCE_FILES})
target_link_libraries(${moduleName} ${libName}) #link target to our library
set_target_properties(${moduleName}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${binDir}/${executablesDir}/${moduleName}"
        OUTPUT_NAME ${moduleName}
        ) #set output directory & binary file name

SET(moduleLib "${executablesTestLibPrefx}${moduleName}")
add_library(${moduleLib} ${PROJECT_SOURCE_FILES})
set_target_properties(${moduleLib}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${binDir}/${executablesDir}/${moduleName}/${testsDir}"
        LIBRARY_OUTPUT_DIRECTORY "${binDir}/${executablesDir}/${moduleName}/${testsDir}"
        ) #set library destination

target_include_directories(${moduleName} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${testsDir}")
    # make a general targot that builds all tests of this module
    set(moduleTestsTarget "${moduleName}_${testsDir}")
    add_custom_target(${moduleTestsTarget})

    #add test subDirectory
    add_subdirectory(${testsDir})
endif ()
#include source files
SET(PROJECT_SOURCE_FILES "")
INCLUDE (sources.cmake)

#extract modue name
file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} dirName)
string(FIND ${dirName} "/" pos1 REVERSE)
MATH(EXPR pos1 "${pos1}+1")
string(SUBSTRING ${dirName} ${pos1} -1 dirName)

#add executable target for module
SET(testName "${testsDir}_${dirName}")
add_executable(${testName} ${PROJECT_SOURCE_FILES})
set_target_properties(${testName}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${binDir}/${testsDir}/${dirName}"
    OUTPUT_NAME ${dirName}
) #set output directory & binary file name
target_link_libraries(${testName} ${libName} ${usedModules}) #link target to our library



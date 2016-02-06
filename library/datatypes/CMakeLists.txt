#include source files
INCLUDE (sources.cmake)

#extract modue name
file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} moduleName)
string(FIND ${moduleName} "/" pos1 REVERSE)
MATH(EXPR pos1 "${pos1}+1")
string(SUBSTRING ${moduleName} ${pos1} -1 moduleName)

set(targetPrfx "${libName}_${moduleName}") #traget prefix



if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${testsDir}")
  # make a general targot that builds all tests of this module
  set(moduleTestsTarget "${targetPrfx}_${testsDir}") 
  add_custom_target(${moduleTestsTarget}) 

  #add test subDirectory
  add_subdirectory(${testsDir})
endif()

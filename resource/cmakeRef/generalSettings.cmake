set(libName zdstools)

set(binDir ${CMAKE_BINARY_DIR}/bin)
set(testsDir "tests")
set(libDir "library")
set(executablesDir "executables")
set(executablesTestLibPrefx "_testlib_")

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(NOT child STREQUAL "Build")
      IF(IS_DIRECTORY ${curdir}/${child})
	  LIST(APPEND dirlist ${child})
      ENDIF()
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO(SUBDIRLIST)

MACRO(AddSourceFilesP)
  SET (allFiles "")
  foreach(sfile ${ARGN})
    SET(allFiles ${allFiles} "${CMAKE_CURRENT_SOURCE_DIR}/${sfile}")  
  ENDFOREACH()
#    message(STATUS "file added " ${allFiles})
    SET(PROJECT_SOURCE_FILES ${PROJECT_SOURCE_FILES}
	  ${allFiles}
	  PARENT_SCOPE
    )
ENDMACRO(AddSourceFilesP)

MACRO(AddSourceFiles)
  SET (allFiles "")
  foreach(sfile ${ARGN})
    SET(allFiles ${allFiles} "${CMAKE_CURRENT_SOURCE_DIR}/${sfile}")  
  ENDFOREACH()
#    message(STATUS "file added " ${allFiles})
    SET(PROJECT_SOURCE_FILES ${PROJECT_SOURCE_FILES}
	  ${allFiles}
    )
ENDMACRO(AddSourceFiles)

MACRO(AddModules)
  SET (usedModules "")
  foreach(sfile ${ARGN})
    SET(usedModules ${usedModules} "${executablesTestLibPrefx}${sfile}")  
  ENDFOREACH()
ENDMACRO(AddModules)
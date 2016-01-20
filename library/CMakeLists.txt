#Add this path to Include paths
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#add subdirectories
SET(PROJECT_SOURCE_FILES "")
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
FOREACH(subdir ${SUBDIRS})
    ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()
#add library target
add_library(${libName} ${PROJECT_SOURCE_FILES})
set_target_properties( ${libName}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${binDir}/${libDir}"
    LIBRARY_OUTPUT_DIRECTORY "${binDir}/${libDir}"
) #set library destination
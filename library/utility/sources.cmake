#list of files to compile
#SET (PROJECT_SOURCE_FILES ${PROJECT_SOURCE_FILES}
#	${CMAKE_CURRENT_SOURCE_DIR}/func1.cpp 
#	${CMAKE_CURRENT_SOURCE_DIR}/func2.cpp
#	PARENT_SCOPE
#)

AddSourceFilesP(utility.cpp jsonParser.cpp FileSystem.cpp)

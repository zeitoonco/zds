#get list of all code files
file(GLOB files "*.cpp")
foreach(file ${files})
  #make target name
  file(TO_NATIVE_PATH ${file} file)
  get_filename_component(fileNO ${file} NAME_WE)
  SET(exeName "${moduleTestsTarget}_${fileNO}")
  #add a target for each test source file
  add_executable(${exeName} ${file})
  target_link_libraries(${exeName} ${libName}) #link target to our library
  add_dependencies(${moduleTestsTarget} ${exeName}) #add this target to this module 
  set_target_properties(${exeName} 
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${binDir}/${libDir}/${testsDir}/${moduleName}"
    OUTPUT_NAME ${fileNO}
  ) #set output directory & binary file name
endforeach()
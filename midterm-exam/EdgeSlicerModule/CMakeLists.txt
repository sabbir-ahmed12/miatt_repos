
set(EdgeSlicerModule_SRC EdgeSlicerModule.cxx)
GENERATECLP(EdgeSlicerModule_SRC EdgeSlicerModule.xml)

## Build stand-alone command line module.
add_executable(${MYHAWKID}EdgeSlicerModule ${EdgeSlicerModule_SRC})
target_link_libraries(${MYHAWKID}EdgeSlicerModule ${ITK_LIBRARIES})

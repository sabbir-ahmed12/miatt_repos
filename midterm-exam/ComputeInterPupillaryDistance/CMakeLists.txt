
set(ComputeInterPupillaryDistance_SRC ComputeInterPupillaryDistance.cxx)
GENERATECLP(ComputeInterPupillaryDistance_SRC ComputeInterPupillaryDistance.xml)

## Build stand-alone command line module.
add_executable(${MYHAWKID}ComputeInterPupillaryDistance ${ComputeInterPupillaryDistance_SRC})
target_link_libraries(${MYHAWKID}ComputeInterPupillaryDistance ${ITK_LIBRARIES}) # <- TODO Add necessary libraries here

set(ImageCoordinates_SOURCE ImageCoordinates_sahmed8.cxx)
GENERATECLP(ImageCoordinates_SOURCE ImageCoordinates_sahmed8.xml)

add_executable(ImageCoordinates_${MYHAWKID} ${ImageCoordinates_SOURCE})
target_link_libraries(ImageCoordinates_${MYHAWKID} ${ITK_LIBRARIES})

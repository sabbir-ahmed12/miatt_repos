# --- Add executable file ---
add_executable(HW03 HW03.cxx)

# --- Link ITK libraries ---
target_link_libraries(HW03 PRIVATE ${ITK_INTERFACE_LIBRARIES})


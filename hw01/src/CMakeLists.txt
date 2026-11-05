# --- Add the executable file ---
add_executable(ece5940_hw01 ece5940_hw01.cxx)

# --- Link ITK libraries ---
target_link_libraries(ece5940_hw01 PRIVATE ${ITK_INTERFACE_LIBRARIES})

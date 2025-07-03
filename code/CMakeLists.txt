cmake_minimum_required(VERSION 3.10)
project(MilkDrop3)

# Add subdirectories
add_subdirectory(audio)
add_subdirectory(ns-eel2)
add_subdirectory(vis_milk2)

# Define the executable target
# For now, plugin.cpp from vis_milk2 is used as a placeholder for the main source file.
add_executable(MilkDrop3 vis_milk2/plugin.cpp)

# Link libraries to the executable
target_link_libraries(MilkDrop3 PRIVATE milkdrop_audio milkdrop_nseel milkdrop_vis)

# Basic include directories for the executable
target_include_directories(MilkDrop3 PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/vis_milk2"
    "${CMAKE_CURRENT_SOURCE_DIR}/audio"
    "${CMAKE_CURRENT_SOURCE_DIR}/ns-eel2"
)

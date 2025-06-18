add_library(milkdrop_vis
    Milkdrop2PcmVisualizer.cpp
    dxcontext.cpp
    fft.cpp
    menu.cpp
    milkdropfs.cpp
    plugin.cpp
    pluginshell.cpp
    state.cpp
    support.cpp
    texmgr.cpp
    textmgr.cpp
    utility.cpp
    wasabi.cpp
)

target_include_directories(milkdrop_vis PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_BINARY_DIR}/.." # Resolves to code/
    # Alternative for code/: "${CMAKE_SOURCE_DIR}" if CMAKE_SOURCE_DIR is .../code
    # For this subtask, assuming CMAKE_SOURCE_DIR is the root of the project, not code/.
    # If build is in code/build, then CMAKE_BINARY_DIR/.. is code/.
    # Let's use a relative path for now, assuming build dir is inside code/
    #  "${CMAKE_CURRENT_SOURCE_DIR}/.." should also point to code/
)

# A more robust way to refer to the "code/" directory from "code/vis_milk2/"
# is to use CMAKE_SOURCE_DIR if it's set to the project root (containing the top CMakeLists.txt)
# or use relative paths if the structure is fixed.
# The subtask asks for "code/", which implies an include path to the parent of vis_milk2.
target_include_directories(milkdrop_vis PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/.." # This should point to the 'code' directory.
    "${CMAKE_CURRENT_SOURCE_DIR}"    # This points to 'code/vis_milk2'
)

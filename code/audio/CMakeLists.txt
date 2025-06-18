add_library(milkdrop_audio
    audiobuf.cpp
    guid.cpp
    log.cpp
    loopback-capture.cpp
    prefs.cpp
)

target_include_directories(milkdrop_audio PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Platform detection and compile definitions

if(WIN32)
    add_compile_definitions(IPTV_OS_WINDOWS)
    message(STATUS "Target platform: Windows")
elseif(APPLE)
    add_compile_definitions(IPTV_OS_MACOS)
    message(STATUS "Target platform: macOS")
elseif(UNIX)
    add_compile_definitions(IPTV_OS_LINUX)
    message(STATUS "Target platform: Linux")
else()
    message(WARNING "Unknown platform")
endif()

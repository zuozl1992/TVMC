# FindFFmpeg.cmake - Cross-platform FFmpeg discovery
#
# This module finds FFmpeg libraries and creates imported targets:
#   FFmpeg::avformat
#   FFmpeg::avcodec
#   FFmpeg::avutil
#   FFmpeg::swscale
#
# Strategy:
#   1. Try pkg-config (Linux, macOS with Homebrew)
#   2. Fall back to FFMPEG_DIR cache variable (Windows)

# Try pkg-config first
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(AVFORMAT QUIET IMPORTED_TARGET libavformat)
    pkg_check_modules(AVCODEC QUIET IMPORTED_TARGET libavcodec)
    pkg_check_modules(AVUTIL QUIET IMPORTED_TARGET libavutil)
    pkg_check_modules(SWSCALE QUIET IMPORTED_TARGET libswscale)
endif()

# If pkg-config found everything, create aliases
if(AVFORMAT_FOUND AND AVCODEC_FOUND AND AVUTIL_FOUND AND SWSCALE_FOUND)
    message(STATUS "FFmpeg found via pkg-config")
    
    # Create unified targets if not already defined
    if(NOT TARGET FFmpeg::avformat)
        add_library(FFmpeg::avformat ALIAS PkgConfig::AVFORMAT)
    endif()
    if(NOT TARGET FFmpeg::avcodec)
        add_library(FFmpeg::avcodec ALIAS PkgConfig::AVCODEC)
    endif()
    if(NOT TARGET FFmpeg::avutil)
        add_library(FFmpeg::avutil ALIAS PkgConfig::AVUTIL)
    endif()
    if(NOT TARGET FFmpeg::swscale)
        add_library(FFmpeg::swscale ALIAS PkgConfig::SWSCALE)
    endif()
else()
    # Fall back to manual path
    if(WIN32)
        set(FFMPEG_DIR "" CACHE PATH "FFmpeg installation directory (e.g., C:/ffmpeg)")
    elseif(APPLE)
        # Auto-detect Homebrew prefix
        execute_process(
            COMMAND brew --prefix ffmpeg
            OUTPUT_VARIABLE HOMEBREW_FFMPEG_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if(HOMEBREW_FFMPEG_PREFIX AND EXISTS "${HOMEBREW_FFMPEG_PREFIX}")
            set(FFMPEG_DIR "${HOMEBREW_FFMPEG_PREFIX}" CACHE PATH "FFmpeg installation directory")
        endif()
    endif()
    
    if(NOT FFMPEG_DIR)
        message(FATAL_ERROR 
            "FFmpeg not found. Install FFmpeg and ensure pkg-config can find it, "
            "or set FFMPEG_DIR to the FFmpeg installation directory.\n"
            "  Linux: sudo apt install libavformat-dev libavcodec-dev libavutil-dev libswscale-dev\n"
            "  macOS: brew install ffmpeg\n"
            "  Windows: Download FFmpeg and set -DFFMPEG_DIR=C:/path/to/ffmpeg"
        )
    endif()
    
    message(STATUS "FFmpeg found at: ${FFMPEG_DIR}")
    
    # Set paths
    set(FFMPEG_INCLUDE_DIRS "${FFMPEG_DIR}/include")
    
    if(WIN32)
        set(FFMPEG_LIBRARY_DIRS "${FFMPEG_DIR}/lib")
        set(FFMPEG_BINARY_DIRS "${FFMPEG_DIR}/bin")
    else()
        set(FFMPEG_LIBRARY_DIRS "${FFMPEG_DIR}/lib")
    endif()
    
    # Create imported targets
    foreach(_lib avformat avcodec avutil swscale)
        if(NOT TARGET FFmpeg::${_lib})
            add_library(FFmpeg::${_lib} SHARED IMPORTED)
            set_target_properties(FFmpeg::${_lib} PROPERTIES
                IMPORTED_LOCATION "${FFMPEG_LIBRARY_DIRS}/lib${_lib}${CMAKE_SHARED_LIBRARY_SUFFIX}"
                INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
            )
            
            # Platform-specific library naming
            if(WIN32)
                set_target_properties(FFmpeg::${_lib} PROPERTIES
                    IMPORTED_IMPLIB "${FFMPEG_LIBRARY_DIRS}/${_lib}.lib"
                    IMPORTED_LOCATION "${FFMPEG_BINARY_DIRS}/${_lib}-${CMAKE_SHARED_LIBRARY_SUFFIX}"
                )
            elseif(APPLE)
                # Homebrew may version the dylib
                file(GLOB _lib_candidates "${FFMPEG_LIBRARY_DIRS}/lib${_lib}*.dylib")
                if(_lib_candidates)
                    list(GET _lib_candidates 0 _lib_path)
                    set_target_properties(FFmpeg::${_lib} PROPERTIES
                        IMPORTED_LOCATION "${_lib_path}"
                    )
                endif()
            endif()
        endif()
    endforeach()
endif()

# Verify all targets exist
foreach(_lib avformat avcodec avutil swscale)
    if(NOT TARGET FFmpeg::${_lib})
        message(FATAL_ERROR "FFmpeg::${_lib} target not created")
    endif()
endforeach()

message(STATUS "FFmpeg targets: FFmpeg::avformat, FFmpeg::avcodec, FFmpeg::avutil, FFmpeg::swscale")

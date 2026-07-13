cmake_minimum_required(VERSION 3.19)
project(IPTVScanner VERSION 2.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
    set(FFMPEG_DIR D:/WorkSpace/ThirdParty/FFmpeg/win_64_8_1_2)
endif()

# Platform detection and FFmpeg discovery
include(cmake/Platform.cmake)
include(cmake/FindFFmpeg.cmake)

# Qt6
find_package(Qt6 6.8 REQUIRED COMPONENTS Core Quick QuickControls2 LinguistTools)

# Application modules
add_subdirectory(src/core)
add_subdirectory(src/multimedia)
add_subdirectory(src/logic)
add_subdirectory(src/platform)
add_subdirectory(src/ui)

qt_standard_project_setup()

# Main executable
qt_add_executable(IPTVScanner
    WIN32 MACOSX_BUNDLE
    src/main.cpp
)

# App icon
if(APPLE)
    set(APP_ICON resources/AppIcon.icns)
    set_source_files_properties(${APP_ICON} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    target_sources(IPTVScanner PRIVATE ${APP_ICON})
    set_target_properties(IPTVScanner PROPERTIES
        MACOSX_BUNDLE_ICON_FILE AppIcon.icns
    )
endif()

target_link_libraries(IPTVScanner PRIVATE
    iptv_core
    iptv_multimedia
    iptv_logic
    iptv_ui
    Qt::Core Qt::Quick Qt::QuickControls2
)

# Translations
qt_add_translations(TARGETS IPTVScanner TS_FILES
    resources/translations/IPTVScanner_zh_CN.ts)

# Tests
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install
include(GNUInstallDirs)
install(TARGETS IPTVScanner
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Deploy script (Qt specific)
qt_generate_deploy_app_script(
    TARGET IPTVScanner
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# Copy FFmpeg libraries into app bundle on macOS
if(APPLE)
    execute_process(
        COMMAND brew --prefix ffmpeg
        OUTPUT_VARIABLE FFMPEG_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(FFMPEG_PREFIX AND EXISTS "${FFMPEG_PREFIX}/lib")
        set(FFMPEG_LIB_PATH "${FFMPEG_PREFIX}/lib")

        file(GLOB FFMPEG_DYLIBS
            "${FFMPEG_LIB_PATH}/libavformat*.dylib"
            "${FFMPEG_LIB_PATH}/libavcodec*.dylib"
            "${FFMPEG_LIB_PATH}/libavutil*.dylib"
            "${FFMPEG_LIB_PATH}/libswscale*.dylib"
            "${FFMPEG_LIB_PATH}/libswresample*.dylib"
        )

        foreach(dylib ${FFMPEG_DYLIBS})
            get_filename_component(lib_name ${dylib} NAME)
            add_custom_command(TARGET IPTVScanner POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${dylib}"
                    "$<TARGET_BUNDLE_DIR:IPTVScanner>/Contents/Frameworks/${lib_name}"
                COMMENT "Copying ${lib_name}"
            )
        endforeach()

        add_custom_command(TARGET IPTVScanner POST_BUILD
            COMMAND bash "${CMAKE_SOURCE_DIR}/scripts/fix_ffmpeg_rpaths.sh"
                "$<TARGET_BUNDLE_DIR:IPTVScanner>"
                "${FFMPEG_LIB_PATH}"
            COMMENT "Fixing FFmpeg library paths"
        )
    endif()
endif()

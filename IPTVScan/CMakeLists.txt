cmake_minimum_required(VERSION 3.19)
project(IPTVScan LANGUAGES CXX)
if(WIN32)
    set(WINDOWS TRUE)
    set(MACOS FALSE)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Current platform is Linux")
elseif(APPLE)
    set(WINDOWS FALSE)
    set(MACOS TRUE)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()
IF(MACOS)
    set(FFMPEG_DIR /opt/homebrew/Cellar/ffmpeg/8.1.1)
ELSEIF(WINDOWS)
    set(FFMPEG_DIR D:/WorkSpace/ThirdParty/FFmpeg/win_64_8_1_2)
ENDIF()
# 相当于-I
include_directories(${FFMPEG_DIR}/include)
# 相当于-L
link_directories(${FFMPEG_DIR}/lib)
find_package(Qt6 6.8 REQUIRED COMPONENTS Core Widgets LinguistTools)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/AppIcon.icns")

qt_add_executable(IPTVScan
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainpage.cpp
    mainpage.h
    mainpage.ui
    scanthread.h scanthread.cpp
    mainthread.h mainthread.cpp
    ${APP_ICON}
)

set_target_properties(IPTVScan PROPERTIES
    MACOSX_BUNDLE_ICON_FILE AppIcon
    MACOSX_BUNDLE_ICON_NAME AppIcon
    MACOSX_BUNDLE_BUNDLE_NAME "IPTVScan"
    MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    MACOSX_BUNDLE_COPYRIGHT "Copyright © 2026"
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
)

set_source_files_properties(${APP_ICON} PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources"
)

qt_add_translations(IPTVScan TS_FILES IPTVScan_en_US.ts)

qt_add_translations(
    TARGETS IPTVScan
    TS_FILES IPTVScan_zh_CN.ts
)
# 添加动态库
target_link_libraries(IPTVScan PRIVATE avformat avutil)

IF(WINDOWS)
    # 配置运行时链接地址rpath，没有这个的话，运行的时候链接不到动态库
    set_target_properties(IPTVScan PROPERTIES
        LINK_FLAGS "${LINK_FLAGS} -Wl,--enable-auto-import,-rpath,${FFMPEG_DIR}/bin"
    )
ENDIF()

target_link_libraries(IPTVScan
    PRIVATE
        Qt::Core
        Qt::Widgets
)
target_link_libraries(IPTVScan PRIVATE Qt6::Core)
target_link_libraries(IPTVScan PRIVATE Qt6::Core)

include(GNUInstallDirs)

install(TARGETS IPTVScan
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET IPTVScan
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

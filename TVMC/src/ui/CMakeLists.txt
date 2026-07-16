#用户界面模块 - QML界面和桥接类
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(iptv_ui STATIC
    manager_backend.h
    manager_backend.cpp
    scanner_backend.h
    scanner_backend.cpp
    language_manager.h
    language_manager.cpp
    frame_image_provider.h
    frame_image_provider.cpp
    logo_image_provider.h
    logo_image_provider.cpp
    setup_helper.h
    setup_helper.cpp
    models/check_list_model.h
    models/check_list_model.cpp
    models/channel_table_model.h
    models/channel_table_model.cpp
    models/source_table_model.h
    models/source_table_model.cpp
    models/scan_result_model.h
    models/scan_result_model.cpp
)

#QML资源文件
qt_add_resources(iptv_ui "tvmc-qml"
    PREFIX "/"
    BASE "${CMAKE_SOURCE_DIR}/resources"
    FILES
        "${CMAKE_SOURCE_DIR}/resources/qml/main.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/Theme.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/tabs/BrowseTab.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/tabs/EditTab.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/tabs/CheckTab.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/tabs/SettingsTab.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/tabs/ScanTab.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/dialogs/DbSetupDialog.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/dialogs/InfoDialog.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/IptvButton.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/IptvSmallButton.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/IptvProgressBar.qml"
)

target_include_directories(iptv_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(iptv_ui PUBLIC
    Qt::Core Qt::Quick Qt::QuickControls2 Qt::Sql Qt::Network
    iptv_core iptv_database iptv_network iptv_multimedia iptv_export iptv_logic
)

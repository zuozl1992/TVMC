#用户界面模块 - QML界面和桥接类
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(iptv_ui STATIC
    manager_backend.cpp
    video_frame_provider.cpp
    frame_image_provider.cpp
    logo_image_provider.cpp
    setup_helper.cpp
    models/check_list_model.cpp
    models/channel_table_model.cpp
    models/source_table_model.cpp
)

#QML资源文件（使用相对路径）
qt_add_resources(iptv_ui "manager-qml"
    PREFIX "/"
    BASE "${CMAKE_SOURCE_DIR}/resources"
    FILES
        "${CMAKE_SOURCE_DIR}/resources/qml/main.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/Theme.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/tabs/BrowseTab.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/tabs/EditTab.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/tabs/CheckTab.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/tabs/SettingsTab.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/dialogs/DbSetupDialog.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/dialogs/InfoDialog.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/IptvButton.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/IptvSmallButton.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/IptvCheckBox.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/IptvComboBox.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/IptvProgressBar.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/IptvRadioButton.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/IptvSpinBox.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/IptvTableView.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/IptvTextField.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/RichTextComboBox.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/VideoPreview.qml"
)

target_include_directories(iptv_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(iptv_ui PUBLIC
    Qt::Core Qt::Quick Qt::QuickControls2 Qt::Sql Qt::Network
    iptv_core iptv_database iptv_network iptv_multimedia iptv_export iptv_logic
)

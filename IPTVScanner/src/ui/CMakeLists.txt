#用户界面模块 - QML界面和桥接类
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(iptv_ui STATIC
    scanner_backend.cpp
    models/scan_result_model.cpp
)

#QML资源文件（使用相对路径）
qt_add_resources(iptv_ui "scanner-qml"
    PREFIX "/"
    BASE "${CMAKE_SOURCE_DIR}/resources"
    FILES
        "${CMAKE_SOURCE_DIR}/resources/qml/main.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/Theme.qml"
        "${CMAKE_SOURCE_DIR}/resources/qml/IptvComponents/IptvButton.qml"
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
    Qt::Core Qt::Quick Qt::QuickControls2
    iptv_core iptv_multimedia iptv_logic
)

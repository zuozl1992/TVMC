add_library(iptv_export STATIC
    exporter.h
    m3u_exporter.h
    m3u_exporter.cpp
    txt_exporter.h
    txt_exporter.cpp
    csv_exporter.h
    csv_exporter.cpp
    xlsx_exporter.h
    xlsx_exporter.cpp
    scanfile_generator.h
    scanfile_generator.cpp
)

target_include_directories(iptv_export PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(iptv_export PUBLIC Qt::Core Qt::Sql iptv_core QXlsx)

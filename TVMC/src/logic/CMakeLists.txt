add_library(iptv_logic STATIC
    channel_service.h
    channel_service.cpp
    check_service.h
    check_service.cpp
    url_builder.h
    url_builder.cpp
    tvlist_builder.h
    tvlist_builder.cpp
    guide_manager.h
    guide_manager.cpp
    scan_service.h
    scan_service.cpp
)

target_include_directories(iptv_logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(iptv_logic PUBLIC Qt::Core iptv_core iptv_database iptv_multimedia iptv_network)

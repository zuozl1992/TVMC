add_library(iptv_network STATIC
    httpclient.h
    httpclient.cpp
    logo_fetcher.h
    logo_fetcher.cpp
)

target_include_directories(iptv_network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(iptv_network PUBLIC Qt::Core Qt::Network iptv_core)

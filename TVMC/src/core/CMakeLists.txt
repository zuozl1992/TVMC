add_library(iptv_core STATIC
    config/appconfig.h
    config/appconfig.cpp
    config/bootstrap.h
    config/bootstrap.cpp
    config/configkeys.h
    logging/logmanager.h
    logging/logmanager.cpp
    logging/logcategories.h
)

target_include_directories(iptv_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(iptv_core PUBLIC Qt::Core Qt::Gui)

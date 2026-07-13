add_library(iptv_database STATIC
    database_manager.cpp
    schema_manager.cpp
    channel_repository.cpp
    source_repository.cpp
    sql_query_model_qml.cpp
)

target_include_directories(iptv_database PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(iptv_database PUBLIC Qt::Core Qt::Sql iptv_core)

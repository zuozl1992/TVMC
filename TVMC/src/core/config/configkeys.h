#pragma once

#include <QString>

namespace Iptv::Core::ConfigKeys {

// ==================== Manager: 流媒体URL设置 ====================
constexpr const char* HTTP_URL = "set/http_url";
constexpr const char* FILE_URL = "set/file_url";
constexpr const char* FCC_URL = "set/fcc_url";
constexpr const char* FCC_USE = "set/fcc_use";

// ==================== Manager: 组播/扫描设置 ====================
constexpr const char* GROUP_URL = "set/group_url";
constexpr const char* GROUP_PORT = "set/group_port";
constexpr const char* GROUP_TYPE = "set/group_type";
constexpr const char* GROUP_ADD_TEST = "set/group_add_test";

// ==================== Manager: 导出设置 ====================
constexpr const char* OUT_BY_ONE = "set/out_by_one";
constexpr const char* TV_GROUP_SELECT = "set/tv_group_select";
constexpr const char* TV_MERGE = "set/tv_merge";
constexpr const char* OUT_ADD_LOGO = "set/out_add_logo";
constexpr const char* OUT_ADD_HD = "set/out_add_hd";
constexpr const char* OUT_MAX_RES = "set/out_max_res";      ///< 导出最大清晰度

// ==================== Manager: 默认值 ====================
constexpr const char* DEFAULT_HTTP_URL = "http://192.168.1.1:12345";
constexpr const char* DEFAULT_FILE_URL = "http://192.168.1.1:54321";
constexpr const char* DEFAULT_GROUP_URL = "239.49.{0#1#2}";
constexpr const char* DEFAULT_GROUP_PORT = "6000#8000#8008";

// ==================== Scanner: 扫描设置 ====================
constexpr const char* SCAN_URL = "set/url";
constexpr const char* THREAD_MAX = "set/thread_max";
constexpr const char* TIMEOUT = "set/time_out";
constexpr const char* AUTO_STEP = "set/auto_step";
constexpr const char* USE_URL = "set/use_url";
constexpr const char* MODEL_FILE_PATH = "set/modelfile_path";
constexpr const char* ADD_TS = "set/add_ts";
constexpr const char* SLOW_SCAN = "set/slow_scan";

// ==================== Scanner: 默认值 ====================
constexpr int DEFAULT_THREAD_MAX = 8;
constexpr int DEFAULT_TIMEOUT = 500;
constexpr bool DEFAULT_AUTO_STEP = false;
constexpr bool DEFAULT_USE_URL = true;
constexpr bool DEFAULT_ADD_TS = false;
constexpr bool DEFAULT_SLOW_SCAN = false;

} // namespace Iptv::Core::ConfigKeys

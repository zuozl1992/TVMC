#pragma once

#include <QString>

namespace Iptv::Multimedia {

/**
 * @brief 流媒体信息结构体（合并版）
 *        存储视频流的解码/探测信息，兼容Manager和Scanner
 */
struct StreamInfo {
    // ==================== 通用字段 ====================
    int width = 0;              ///< 视频宽度
    int height = 0;             ///< 视频高度
    int fps = 0;                ///< 帧率
    int sourceType = 0;         ///< 源类型（0=未知, 1=TS, 2=SD, 3=HD, 4=4K, 5=8K）

    // ==================== Manager专用 ====================
    uchar *frameData = nullptr; ///< BGRA像素数据（调用方负责释放）
    QString codecName;          ///< 编解码器名称

    // ==================== Scanner专用 ====================
    QString url;                ///< 探测的URL地址
    bool success = false;       ///< 是否探测成功
};

} // namespace Iptv::Multimedia

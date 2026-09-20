#pragma once

#include <QString>

extern "C" {
#include <libavutil/rational.h>
}

namespace Iptv::Multimedia {

/**
 * @brief FFmpeg工具函数集（统一版本）
 *        合并Manager和Scanner的类型映射
 */
namespace FfmpegUtils {
    /** @brief 有理数转浮点数
     *  @param r AVRational结构体
     *  @return 浮点数值 */
    double r2d(AVRational r);

    /** @brief 根据视频宽度推断源类型
     *  @param width 视频宽度
     *  @return 源类型ID（0=未知, 1=TS, 2=SD, 3=HD, 4=4K, 5=8K） */
    int inferSourceType(int width);

    /** @brief 获取源类型的中文名称
     *  @param type 源类型ID
     *  @return 类型名称字符串 */
    QString sourceTypeName(int type);
}

} // namespace Iptv::Multimedia

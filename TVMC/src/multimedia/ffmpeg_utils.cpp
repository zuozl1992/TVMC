#include "ffmpeg_utils.h"

namespace Iptv::Multimedia {

namespace FfmpegUtils {

double r2d(AVRational r)
{
    return r.num == 0 || r.den == 0 ? 0.0 :
               static_cast<double>(r.num) / static_cast<double>(r.den);
}

int inferSourceType(int width)
{
    //根据视频宽度推断源类型（保持与数据库兼容）
    if (width > 7000) return 4;   // 8K
    if (width > 3000) return 3;   // 4K
    if (width > 1000) return 2;   // HD
    return 1;                     // SD
}

QString sourceTypeName(int type)
{
    switch (type) {
    case 0: return "TS";
    case 1: return "SD";
    case 2: return "HD";
    case 3: return "4K";
    case 4: return "8K";
    default: return "Unknown";
    }
}

} // namespace FfmpegUtils

} // namespace Iptv::Multimedia

#pragma once

#include <QQuickImageProvider>
#include <QPixmap>

namespace Iptv::Ui {

/**
 * @brief 帧图像提供者
 *        为QML提供视频帧图像的QQuickImageProvider实现
 */
class FrameImageProvider : public QQuickImageProvider
{
public:
    FrameImageProvider();

    /** @brief 处理QML的图像请求
     *  @param id          图像标识符
     *  @param size        [输出] 图像原始尺寸
     *  @param requestedSize 请求的尺寸
     *  @return QPixmap图像数据 */
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;

    /** @brief 更新帧图像
     *  @param pixmap 新的帧图像 */
    void setFramePixmap(const QPixmap &pixmap);

    /** @brief 清空帧图像 */
    void clearFrame();

private:
    QPixmap m_pixmap;   ///< 当前帧图像
};

} // namespace Iptv::Ui

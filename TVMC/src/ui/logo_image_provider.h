#pragma once

#include <QQuickImageProvider>
#include <QPixmap>

namespace Iptv::Ui {

/**
 * @brief 台标图像提供者
 *        为QML提供台标图像的QQuickImageProvider实现
 */
class LogoImageProvider : public QQuickImageProvider
{
public:
    LogoImageProvider();

    /** @brief 处理QML的图像请求
     *  @param id          图像标识符
     *  @param size        [输出] 图像原始尺寸
     *  @param requestedSize 请求的尺寸
     *  @return QPixmap图像数据 */
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;

    /** @brief 更新台标图像
     *  @param pixmap 新的台标图像 */
    void setLogoPixmap(const QPixmap &pixmap);

    /** @brief 清空台标图像 */
    void clearLogo();

private:
    QPixmap m_pixmap;   ///< 当前台标图像
};

} // namespace Iptv::Ui

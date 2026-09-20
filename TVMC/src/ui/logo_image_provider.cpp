#include "logo_image_provider.h"

namespace Iptv::Ui {

LogoImageProvider::LogoImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap LogoImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id)

    if (size) {
        *size = m_pixmap.size();
    }

    //按请求尺寸缩放
    if (requestedSize.isValid() && !m_pixmap.isNull()) {
        return m_pixmap.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    return m_pixmap;
}

void LogoImageProvider::setLogoPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
}

void LogoImageProvider::clearLogo()
{
    m_pixmap = QPixmap();
}

} // namespace Iptv::Ui

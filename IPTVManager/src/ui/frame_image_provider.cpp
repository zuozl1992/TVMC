#include "frame_image_provider.h"

namespace Iptv::Ui {

FrameImageProvider::FrameImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap FrameImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
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

void FrameImageProvider::setFramePixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
}

void FrameImageProvider::clearFrame()
{
    m_pixmap = QPixmap();
}

} // namespace Iptv::Ui

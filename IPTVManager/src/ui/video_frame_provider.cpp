#include "video_frame_provider.h"

#include <QPainter>

namespace Iptv::Ui {

VideoFrameProvider::VideoFrameProvider(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setFillColor(Qt::black);
}

VideoFrameProvider::~VideoFrameProvider()
{
    clear();
}

void VideoFrameProvider::setFrameData(uchar *data, int width, int height)
{
    clear();

    m_data = data;
    m_width = width;
    m_height = height;

    //创建QImage（BGRA格式对应Format_ARGB32）
    m_image = QImage(m_data, m_width, m_height, m_width * 4, QImage::Format_ARGB32);

    update();
}

void VideoFrameProvider::clear()
{
    if (m_data) {
        delete[] m_data;
        m_data = nullptr;
    }
    m_width = 0;
    m_height = 0;
    m_image = QImage();

    update();
}

void VideoFrameProvider::paint(QPainter *painter)
{
    if (m_image.isNull()) {
        return;
    }

    //缩放图像以适应控件大小
    QRectF target = boundingRect();
    QSizeF imageSize = m_image.size();

    //保持宽高比
    QSizeF scaledSize = imageSize.scaled(target.size(), Qt::KeepAspectRatio);
    QRectF targetRect(
        (target.width() - scaledSize.width()) / 2,
        (target.height() - scaledSize.height()) / 2,
        scaledSize.width(),
        scaledSize.height()
    );

    painter->drawImage(targetRect, m_image);
}

} // namespace Iptv::Ui

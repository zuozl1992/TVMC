#pragma once

#include <QQuickPaintedItem>
#include <QImage>

namespace Iptv::Ui {

/**
 * @brief 视频帧预览组件（QML版）
 *        使用QQuickPaintedItem替代QOpenGLWidget
 */
class VideoFrameProvider : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit VideoFrameProvider(QQuickItem *parent = nullptr);
    ~VideoFrameProvider();

    /** @brief 设置帧数据（BGRA格式，调用后数据由组件管理） */
    Q_INVOKABLE void setFrameData(uchar *data, int width, int height);

    /** @brief 清空画面 */
    Q_INVOKABLE void clear();

protected:
    void paint(QPainter *painter) override;

private:
    uchar *m_data = nullptr;
    int m_width = 0;
    int m_height = 0;
    QImage m_image;
};

} // namespace Iptv::Ui

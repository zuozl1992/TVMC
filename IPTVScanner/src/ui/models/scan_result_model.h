#pragma once

#include "multimedia/stream_info.h"
#include <QAbstractTableModel>
#include <QVector>

namespace Iptv::Ui {

/**
 * @brief 扫描结果表格模型
 *        存储和展示流探测结果数据
 */
class ScanResultModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    /** @brief 列定义 */
    enum Column {
        ColUrl = 0,     ///< URL地址
        ColWidth,       ///< 视频宽度
        ColHeight,      ///< 视频高度
        ColFps,         ///< 帧率
        ColSourceType,  ///< 源类型
        ColumnCount     ///< 列数
    };

    /** @brief QML角色定义 */
    enum Roles {
        UrlRole = Qt::UserRole,
        WidthRole,
        HeightRole,
        FpsRole,
        SourceTypeRole
    };

    explicit ScanResultModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    /** @brief 添加一条扫描结果 */
    void addResult(const Multimedia::StreamInfo &info);

    /** @brief 清空所有结果 */
    void clear();

    /** @brief 获取所有结果 */
    QVector<Multimedia::StreamInfo> results() const;

private:
    QVector<Multimedia::StreamInfo> m_results;  ///< 扫描结果列表
};

} // namespace Iptv::Ui

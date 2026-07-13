#include "scan_result_model.h"
#include "multimedia/ffmpeg_utils.h"

namespace Iptv::Ui {

ScanResultModel::ScanResultModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int ScanResultModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_results.size();
}

int ScanResultModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant ScanResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_results.size())
        return {};

    const Multimedia::StreamInfo &info = m_results.at(index.row());

    //处理自定义角色（QML使用）
    if (role >= UrlRole) {
        switch (role) {
        case UrlRole: return info.url;
        case WidthRole: return info.width;
        case HeightRole: return info.height;
        case FpsRole: return info.fps;
        case SourceTypeRole: return Multimedia::FfmpegUtils::sourceTypeName(info.sourceType);
        }
    }

    //处理标准DisplayRole（TableView使用）
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case ColUrl:
            return info.url;
        case ColWidth:
            return info.width;
        case ColHeight:
            return info.height;
        case ColFps:
            return info.fps;
        case ColSourceType:
            return Multimedia::FfmpegUtils::sourceTypeName(info.sourceType);
        }
    }

    return {};
}

QHash<int, QByteArray> ScanResultModel::roleNames() const
{
    return {
        { UrlRole, "url" },
        { WidthRole, "width" },
        { HeightRole, "height" },
        { FpsRole, "fps" },
        { SourceTypeRole, "sourceType" }
    };
}

QVariant ScanResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    //设置中文列标题
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case ColUrl:
            return tr("URL");
        case ColWidth:
            return tr("Width");
        case ColHeight:
            return tr("Height");
        case ColFps:
            return tr("FPS");
        case ColSourceType:
            return tr("Type");
        }
    }

    return {};
}

void ScanResultModel::addResult(const Multimedia::StreamInfo &info)
{
    //通知视图插入新行
    beginInsertRows(QModelIndex(), m_results.size(), m_results.size());
    m_results.append(info);
    endInsertRows();
}

void ScanResultModel::clear()
{
    //通知视图重置模型
    beginResetModel();
    m_results.clear();
    endResetModel();
}

QVector<Multimedia::StreamInfo> ScanResultModel::results() const
{
    return m_results;
}

} // namespace Iptv::Ui

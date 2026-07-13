#include "check_list_model.h"
#include "multimedia/ffmpeg_utils.h"

namespace Iptv::Ui {

CheckListModel::CheckListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int CheckListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_list.size();
}

QVariant CheckListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_list.size())
        return {};

    QJsonObject obj = m_list.at(index.row()).toObject();
    QString name = obj.value("name").toString();
    QString type = Multimedia::FfmpegUtils::sourceTypeName(obj.value("type").toInt());
    QString ip = obj.value("ip").toString();
    int port = obj.value("port").toInt();

    switch (role) {
    case BracketRole:
        return QString("[%1]").arg(index.row() + 1, 3, 10, QChar(' '));
    case NameRole:
        return name;
    case TypeRole:
        return type;
    case IpPortRole:
        return QString("%1:%2").arg(ip).arg(port);
    case FullTextRole:
        return QString("[%1] %2 | %3 | %4:%5")
            .arg(index.row() + 1, 3, 10, QChar(' '))
            .arg(name, -15)
            .arg(type, -4)
            .arg(ip)
            .arg(port);
    }

    return {};
}

QHash<int, QByteArray> CheckListModel::roleNames() const
{
    return {
        { BracketRole, "bracket" },
        { NameRole, "name" },
        { TypeRole, "type" },
        { IpPortRole, "ipPort" },
        { FullTextRole, "fullText" }
    };
}

void CheckListModel::setList(const QJsonArray &list)
{
    beginResetModel();
    m_list = list;
    endResetModel();
}

QJsonArray CheckListModel::list() const
{
    return m_list;
}

} // namespace Iptv::Ui

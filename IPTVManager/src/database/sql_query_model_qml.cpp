#include "sql_query_model_qml.h"
#include <QSqlRecord>

namespace Iptv::Database {

SqlQueryModelQml::SqlQueryModelQml(QObject *parent)
    : QSqlQueryModel(parent)
{
}

QHash<int, QByteArray> SqlQueryModelQml::roleNames() const
{
    return m_roleNames;
}

QVariant SqlQueryModelQml::data(const QModelIndex &index, int role) const
{
    //标准角色直接返回
    if (role < Qt::UserRole) {
        return QSqlQueryModel::data(index, role);
    }

    //自定义角色：根据列索引返回数据
    int column = role - Qt::UserRole;
    QModelIndex idx = this->index(index.row(), column);
    return QSqlQueryModel::data(idx, Qt::DisplayRole);
}

void SqlQueryModelQml::setQuerySql(const QString &sql, QSqlDatabase db)
{
    setQuery(sql, db);

    //根据查询结果构建角色名映射
    m_roleNames.clear();
    QSqlRecord rec = record();
    for (int i = 0; i < rec.count(); ++i) {
        m_roleNames.insert(Qt::UserRole + i, rec.fieldName(i).toUtf8());
    }

    //获取所有行（QSqlQueryModel默认只获取部分数据）
    while (canFetchMore()) {
        fetchMore();
    }
}

} // namespace Iptv::Database

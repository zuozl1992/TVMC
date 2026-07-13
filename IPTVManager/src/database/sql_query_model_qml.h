#pragma once

#include <QSqlQueryModel>
#include <QHash>
#include <QByteArray>

namespace Iptv::Database {

/**
 * @brief QML兼容的SQL查询模型
 *        为QSqlQueryModel添加roleNames()支持，使其可在QML中使用
 */
class SqlQueryModelQml : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit SqlQueryModelQml(QObject *parent = nullptr);

    /** @brief 返回角色名映射（QML需要） */
    QHash<int, QByteArray> roleNames() const override;

    /** @brief 根据角色返回数据 */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /** @brief 设置查询SQL并自动构建角色名映射
     *  @param sql SQL查询语句
     *  @param db  数据库连接 */
    Q_INVOKABLE void setQuerySql(const QString &sql, QSqlDatabase db);

private:
    QHash<int, QByteArray> m_roleNames;  ///< 角色名映射
};

} // namespace Iptv::Database

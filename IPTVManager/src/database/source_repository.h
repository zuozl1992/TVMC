#pragma once

#include "types.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>

namespace Iptv::Database {

/**
 * @brief 信号源数据仓库
 *        封装tv_source_info表的所有数据库操作
 */
class SourceRepository
{
public:
    explicit SourceRepository(QSqlDatabase *db);

    /** @brief 插入信号源记录 */
    bool insertSource(const TvSource &source);

    /** @brief 更新信号源记录 */
    bool updateSource(const TvSource &source);

    /** @brief 插入或更新信号源（按IP去重） */
    bool upsertSource(const TvSource &source);

    /** @brief 删除指定IP的信号源 */
    bool deleteByIp(const QString &ip);

    /** @brief 删除所有信号源 */
    bool deleteAll();

    /** @brief 检查指定IP是否存在 */
    bool existsByIp(const QString &ip);

    /** @brief 获取所有信号源IP列表 */
    QStringList allIps();

    /** @brief 获取TS类型信号源IP列表 */
    QStringList tsIps();

    /** @brief 获取检测列表
     *  @param normalType true=正常源，false=异常源
     *  @param order      排序方式（0=按IP，1=按分辨率） */
    QJsonArray getCheckList(bool normalType, int order);

    /** @brief 按分组获取频道列表
     *  @param group   分组名称
     *  @param withLogo 是否包含台标信息 */
    QJsonArray getTvListByGroup(const QString &group, bool withLogo);

    /** @brief 获取测试频道URL列表
     *  @param port 默认端口号
     *  @return 格式为ip:port的URL列表 */
    QStringList getTestUrls(const QString &port);

    /** @brief 创建联表查询模型（用于主表格显示） */
    QSqlQueryModel *createJoinedTableModel(QObject *parent);

private:
    QSqlDatabase *m_db; ///< 数据库连接指针
};

} // namespace Iptv::Database

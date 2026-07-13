#include "source_repository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>

namespace Iptv::Database {

SourceRepository::SourceRepository(QSqlDatabase *db)
    : m_db(db)
{
}

bool SourceRepository::insertSource(const TvSource &source)
{
    QSqlQuery query(*m_db);
    //自动生成ID并插入信号源记录
    query.prepare(
        "INSERT INTO tv_source_info (id, tv_id, ip, port, width, height, fps, last_check_date, type, notes) "
        "VALUES (COALESCE((SELECT MAX(id)+1 FROM tv_source_info), 0), :tv_id, :ip, :port, :width, :height, :fps, :last_check_date, :type, :notes)");
    query.bindValue(":tv_id", source.tvId);
    query.bindValue(":ip", source.ip);
    query.bindValue(":port", source.port);
    query.bindValue(":width", source.width);
    query.bindValue(":height", source.height);
    query.bindValue(":fps", source.fps);
    query.bindValue(":last_check_date", source.lastCheckDate.isValid() ? source.lastCheckDate : QDateTime::currentDateTime());
    query.bindValue(":type", source.type);
    query.bindValue(":notes", source.notes);
    
    if (!query.exec()) {
        qWarning() << "Failed to insert source:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool SourceRepository::updateSource(const TvSource &source)
{
    QSqlQuery query(*m_db);
    query.prepare(
        "UPDATE tv_source_info SET tv_id = :tv_id, port = :port, width = :width, "
        "height = :height, fps = :fps, last_check_date = :last_check_date, "
        "type = :type, notes = :notes WHERE ip = :ip");
    query.bindValue(":tv_id", source.tvId);
    query.bindValue(":port", source.port);
    query.bindValue(":width", source.width);
    query.bindValue(":height", source.height);
    query.bindValue(":fps", source.fps);
    query.bindValue(":last_check_date", QDateTime::currentDateTime());
    query.bindValue(":type", source.type);
    query.bindValue(":notes", source.notes);
    query.bindValue(":ip", source.ip);
    
    if (!query.exec()) {
        qWarning() << "Failed to update source:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool SourceRepository::upsertSource(const TvSource &source)
{
    //按IP判断插入或更新
    if (existsByIp(source.ip)) {
        return updateSource(source);
    } else {
        return insertSource(source);
    }
}

bool SourceRepository::deleteByIp(const QString &ip)
{
    QSqlQuery query(*m_db);
    query.prepare("DELETE FROM tv_source_info WHERE ip = :ip");
    query.bindValue(":ip", ip);
    
    if (!query.exec()) {
        qWarning() << "Failed to delete source:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool SourceRepository::deleteAll()
{
    QSqlQuery query(*m_db);
    if (!query.exec("DELETE FROM tv_source_info")) {
        qWarning() << "Failed to delete all sources:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool SourceRepository::existsByIp(const QString &ip)
{
    QSqlQuery query(*m_db);
    query.prepare("SELECT 1 FROM tv_source_info WHERE ip = :ip");
    query.bindValue(":ip", ip);
    
    return query.exec() && query.next();
}

QStringList SourceRepository::allIps()
{
    QSqlQuery query(*m_db);
    if (!query.exec("SELECT ip FROM tv_source_info")) {
        return {};
    }
    
    QStringList ips;
    while (query.next()) {
        ips.append(query.value(0).toString());
    }
    
    return ips;
}

QStringList SourceRepository::tsIps()
{
    QSqlQuery query(*m_db);
    //查询TS类型（type=0）的信号源IP
    if (!query.exec("SELECT DISTINCT ip FROM tv_source_info WHERE type = 0")) {
        return {};
    }
    
    QStringList ips;
    while (query.next()) {
        ips.append(query.value(0).toString());
    }
    
    return ips;
}

QJsonArray SourceRepository::getCheckList(bool normalType, int order)
{
    QSqlQuery query(*m_db);
    
    //构建联表查询SQL
    QString sql = "SELECT tsi.id, tsi.tv_id, tsi.ip, tsi.port, tsi.width, tsi.height, "
                  "tsi.fps, tsi.last_check_date, tsi.type, tsi.notes, ti.name, ti.channel_id "
                  "FROM tv_source_info tsi "
                  "JOIN tv_info ti ON ti.id = tsi.tv_id ";
    
    //按类型过滤
    if (normalType) {
        sql += "WHERE tsi.type != 0 ";
    } else {
        sql += "WHERE tsi.type = 0 ";
    }
    
    //按指定方式排序
    if (order == 0) {
        //按IP自然排序
        sql += "ORDER BY json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[0]'), "
               "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[1]'), "
               "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[2]'), "
               "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[3]')";
    } else {
        //按频道编号和分辨率排序
        sql += "ORDER BY ti.channel_id, (tsi.width * tsi.height) DESC";
    }
    
    if (!query.exec(sql)) {
        qWarning() << "Failed to get check list:" << query.lastError().text();
        return {};
    }
    
    QJsonArray list;
    while (query.next()) {
        QJsonObject obj;
        obj["id"] = query.value(0).toInt();
        obj["tv_id"] = query.value(1).toInt();
        obj["ip"] = query.value(2).toString();
        obj["port"] = query.value(3).toInt();
        obj["width"] = query.value(4).toInt();
        obj["height"] = query.value(5).toInt();
        obj["fps"] = query.value(6).toInt();
        obj["last_check_date"] = query.value(7).toString();
        obj["type"] = query.value(8).toInt();
        obj["notes"] = query.value(9).toString();
        obj["name"] = query.value(10).toString();
        obj["channel_id"] = query.value(11).toInt();
        list.append(obj);
    }
    
    return list;
}

QJsonArray SourceRepository::getTvListByGroup(const QString &group, bool withLogo)
{
    QSqlQuery query(*m_db);
    
    //按分组查询频道列表
    QString sql;
    if (withLogo) {
        sql = "SELECT ti.channel_id, ti.name, tsi.ip, tsi.port, tsi.type, ti.logo_name "
              "FROM tv_info ti "
              "JOIN tv_source_info tsi ON ti.id = tsi.tv_id "
              "WHERE ti.\"group\" = :group "
              "ORDER BY ti.channel_id, tsi.type DESC, tsi.fps DESC, "
              "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[0]')";
    } else {
        sql = "SELECT ti.channel_id, ti.name, tsi.ip, tsi.port "
              "FROM tv_info ti "
              "JOIN tv_source_info tsi ON ti.id = tsi.tv_id "
              "WHERE ti.\"group\" = :group "
              "ORDER BY ti.channel_id, tsi.type DESC, tsi.fps DESC, "
              "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[0]')";
    }
    
    query.prepare(sql);
    query.bindValue(":group", group);
    
    if (!query.exec()) {
        qWarning() << "Failed to get TV list:" << query.lastError().text();
        return {};
    }
    
    QJsonArray list;
    while (query.next()) {
        QJsonObject obj;
        obj["channel_id"] = query.value(0).toInt();
        obj["name"] = query.value(1).toString();
        obj["ip"] = query.value(2).toString();
        obj["port"] = query.value(3).toInt();
        obj["group"] = group;
        
        if (withLogo) {
            obj["type"] = query.value(4).toInt();
            obj["logo_name"] = query.value(5).toString();
        }
        
        list.append(obj);
    }
    
    return list;
}

QStringList SourceRepository::getTestUrls(const QString &port)
{
    QSqlQuery query(*m_db);
    //查询测试频道（tv_id=0）并按IP自然排序
    query.prepare("SELECT ip, port FROM tv_source_info WHERE tv_id = 0 ORDER BY "
                  "json_extract('[' || replace(ip, '.', ',') || ']', '$[0]'), "
                  "json_extract('[' || replace(ip, '.', ',') || ']', '$[1]'), "
                  "json_extract('[' || replace(ip, '.', ',') || ']', '$[2]'), "
                  "json_extract('[' || replace(ip, '.', ',') || ']', '$[3]')");
    
    if (!query.exec()) {
        return {};
    }
    
    QStringList urls;
    while (query.next()) {
        QString ip = query.value(0).toString();
        int p = query.value(1).toInt();
        urls.append(QString("%1:%2").arg(ip).arg(p > 0 ? p : port.toInt()));
    }
    
    return urls;
}

QSqlQueryModel *SourceRepository::createJoinedTableModel(QObject *parent)
{
    QSqlQueryModel *model = new QSqlQueryModel(parent);
    //创建频道-信号源联表查询模型
    model->setQuery(
        "SELECT ti.id, ti.channel_id, ti.name, ti.\"group\", tsi.ip, tsi.port, "
        "tsi.width, tsi.height, tsi.fps, st.name as type, tsi.notes "
        "FROM tv_info ti "
        "JOIN tv_source_info tsi ON ti.id = tsi.tv_id "
        "JOIN source_type st ON st.id = tsi.type "
        "ORDER BY ti.channel_id, tsi.width DESC, tsi.fps DESC",
        *m_db);
    
    //设置中文列标题
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "频道ID");
    model->setHeaderData(2, Qt::Horizontal, "频道");
    model->setHeaderData(3, Qt::Horizontal, "分组");
    model->setHeaderData(4, Qt::Horizontal, "地址");
    model->setHeaderData(5, Qt::Horizontal, "端口");
    model->setHeaderData(6, Qt::Horizontal, "宽");
    model->setHeaderData(7, Qt::Horizontal, "高");
    model->setHeaderData(8, Qt::Horizontal, "FPS");
    model->setHeaderData(9, Qt::Horizontal, "类型");
    model->setHeaderData(10, Qt::Horizontal, "备注");
    
    return model;
}

} // namespace Iptv::Database

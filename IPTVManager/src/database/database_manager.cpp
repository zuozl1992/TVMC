#include "database_manager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

namespace Iptv::Database {

DatabaseManager::DatabaseManager(const QString &dbPath, QObject *parent)
    : QObject(parent)
    , m_dbPath(dbPath)
{
}

DatabaseManager::~DatabaseManager()
{
    close();
}

bool DatabaseManager::open()
{
    if (m_db.isOpen()) {
        return true;
    }
    
    //添加SQLite数据库连接
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_dbPath);
    
    if (!m_db.open()) {
        qCritical() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }
    
    //启用外键约束
    QSqlQuery query(m_db);
    query.exec("PRAGMA foreign_keys = ON");
    
    qInfo() << "Database opened:" << m_dbPath;
    return true;
}

void DatabaseManager::close()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::isOpen() const
{
    return m_db.isOpen();
}

bool DatabaseManager::resetToDefault()
{
    close();
    
    //删除现有数据库文件
    if (QFile::exists(m_dbPath)) {
        QFile::remove(m_dbPath);
    }
    
    //从资源文件复制默认数据库
    QString defaultDb = ":/db_def.db";
    if (!QFile::copy(defaultDb, m_dbPath)) {
        qCritical() << "Failed to copy default database";
        return false;
    }
    
    //设置文件权限
    QFile::setPermissions(m_dbPath, QFile::ReadOwner | QFile::WriteOwner);
    
    //重新打开数据库
    if (!open()) {
        return false;
    }
    
    //清理备份表（以_开头的表）
    QSqlQuery query(m_db);
    QStringList tables = m_db.tables();
    for (const QString &table : tables) {
        if (table.startsWith("_")) {
            query.exec(QString("DROP TABLE IF EXISTS \"%1\"").arg(table));
            qInfo() << "Dropped backup table:" << table;
        }
    }
    
    return true;
}

QSqlDatabase *DatabaseManager::database()
{
    return &m_db;
}

QString DatabaseManager::dbPath() const
{
    return m_dbPath;
}

} // namespace Iptv::Database

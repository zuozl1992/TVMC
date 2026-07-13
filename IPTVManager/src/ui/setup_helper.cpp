#include "setup_helper.h"
#include <QSettings>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QFileInfo>

namespace Iptv::Ui {

SetupHelper::SetupHelper(const QString &bootstrapPath, QObject *parent)
    : QObject(parent)
    , m_bootstrapPath(bootstrapPath)
{
}

void SetupHelper::saveAndQuit(const QString &configPath, const QString &dbPath)
{
    //确保配置文件目录存在
    QFileInfo configInfo(configPath);
    QDir().mkpath(configInfo.absolutePath());

    //如果配置文件不存在，创建一个空的
    if (!QFile::exists(configPath)) {
        QFile configFile(configPath);
        configFile.open(QIODevice::WriteOnly);
        configFile.close();
    }

    //确保数据库文件目录存在
    QFileInfo dbInfo(dbPath);
    QDir().mkpath(dbInfo.absolutePath());

    //如果数据库文件不存在，创建一个空的
    if (!QFile::exists(dbPath)) {
        QFile dbFile(dbPath);
        dbFile.open(QIODevice::WriteOnly);
        dbFile.close();
    }

    //保存bootstrap配置
    if (!m_bootstrapPath.isEmpty()) {
        QSettings settings(m_bootstrapPath, QSettings::IniFormat);
        settings.setValue("paths/config", configPath);
        settings.setValue("paths/db", dbPath);
        settings.sync();
    }
    QCoreApplication::quit();
}

} // namespace Iptv::Ui

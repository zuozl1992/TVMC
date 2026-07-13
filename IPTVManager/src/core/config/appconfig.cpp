#include "appconfig.h"
#include "configkeys.h"
#include <QSettings>

namespace Iptv::Core {

AppConfig *AppConfig::s_instance = nullptr;

AppConfig *AppConfig::instance()
{
    //单例模式：延迟初始化
    if (!s_instance) {
        s_instance = new AppConfig();
    }
    return s_instance;
}

AppConfig::AppConfig(QObject *parent)
    : QObject(parent)
{
}

void AppConfig::init(const QString &configPath)
{
    if (m_settings) {
        delete m_settings;
    }
    //使用INI格式的QSettings管理配置
    m_settings = new QSettings(configPath, QSettings::IniFormat, this);
}

QVariant AppConfig::value(const QString &key, const QVariant &defaultValue) const
{
    if (!m_settings) return defaultValue;
    return m_settings->value(key, defaultValue);
}

void AppConfig::setValue(const QString &key, const QVariant &value)
{
    if (!m_settings) return;
    m_settings->setValue(key, value);
    m_settings->sync();
}

QString AppConfig::httpUrl() const
{
    return value(ConfigKeys::HTTP_URL, ConfigKeys::DEFAULT_HTTP_URL).toString();
}

void AppConfig::setHttpUrl(const QString &url)
{
    setValue(ConfigKeys::HTTP_URL, url);
    emit configChanged();
}

QString AppConfig::fileUrl() const
{
    return value(ConfigKeys::FILE_URL, ConfigKeys::DEFAULT_FILE_URL).toString();
}

void AppConfig::setFileUrl(const QString &url)
{
    setValue(ConfigKeys::FILE_URL, url);
    emit configChanged();
}

QString AppConfig::groupAddress() const
{
    return value(ConfigKeys::GROUP_URL, ConfigKeys::DEFAULT_GROUP_URL).toString();
}

void AppConfig::setGroupAddress(const QString &addr)
{
    setValue(ConfigKeys::GROUP_URL, addr);
    emit configChanged();
}

QString AppConfig::groupPort() const
{
    return value(ConfigKeys::GROUP_PORT, ConfigKeys::DEFAULT_GROUP_PORT).toString();
}

void AppConfig::setGroupPort(const QString &port)
{
    setValue(ConfigKeys::GROUP_PORT, port);
    emit configChanged();
}

int AppConfig::urlTypeIndex() const
{
    return value(ConfigKeys::GROUP_TYPE, 0).toInt();
}

void AppConfig::setUrlTypeIndex(int index)
{
    setValue(ConfigKeys::GROUP_TYPE, index);
    emit configChanged();
}

bool AppConfig::addTestEnabled() const
{
    return value(ConfigKeys::GROUP_ADD_TEST, false).toBool();
}

void AppConfig::setAddTestEnabled(bool enabled)
{
    setValue(ConfigKeys::GROUP_ADD_TEST, enabled);
    emit configChanged();
}

bool AppConfig::outByOne() const
{
    return value(ConfigKeys::OUT_BY_ONE, false).toBool();
}

void AppConfig::setOutByOne(bool enabled)
{
    setValue(ConfigKeys::OUT_BY_ONE, enabled);
    emit configChanged();
}

bool AppConfig::mergeChannels() const
{
    return value(ConfigKeys::TV_MERGE, false).toBool();
}

void AppConfig::setMergeChannels(bool enabled)
{
    setValue(ConfigKeys::TV_MERGE, enabled);
    emit configChanged();
}

bool AppConfig::addLogoEnabled() const
{
    return value(ConfigKeys::OUT_ADD_LOGO, false).toBool();
}

void AppConfig::setAddLogoEnabled(bool enabled)
{
    setValue(ConfigKeys::OUT_ADD_LOGO, enabled);
    emit configChanged();
}

bool AppConfig::addHdSuffix() const
{
    return value(ConfigKeys::OUT_ADD_HD, false).toBool();
}

void AppConfig::setAddHdSuffix(bool enabled)
{
    setValue(ConfigKeys::OUT_ADD_HD, enabled);
    emit configChanged();
}

QString AppConfig::fccUrl() const
{
    return value(ConfigKeys::FCC_URL, QString()).toString();
}

void AppConfig::setFccUrl(const QString &url)
{
    setValue(ConfigKeys::FCC_URL, url);
    emit configChanged();
}

bool AppConfig::fccEnabled() const
{
    return value(ConfigKeys::FCC_USE, false).toBool();
}

void AppConfig::setFccEnabled(bool enabled)
{
    setValue(ConfigKeys::FCC_USE, enabled);
    emit configChanged();
}

QStringList AppConfig::selectedGroups() const
{
    return value(ConfigKeys::TV_GROUP_SELECT, QStringList()).toStringList();
}

void AppConfig::setSelectedGroups(const QStringList &groups)
{
    setValue(ConfigKeys::TV_GROUP_SELECT, groups);
    emit configChanged();
}

} // namespace Iptv::Core

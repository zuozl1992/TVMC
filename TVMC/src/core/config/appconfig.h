#pragma once

#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>

namespace Iptv::Core {

/**
 * @brief 应用配置管理器（单例）
 *        管理所有持久化配置项的读写操作，合并Manager和Scanner配置
 */
class AppConfig : public QObject
{
    Q_OBJECT

    // ==================== Manager QML属性 ====================
    Q_PROPERTY(QString httpUrl READ httpUrl WRITE setHttpUrl NOTIFY configChanged)
    Q_PROPERTY(QString fileUrl READ fileUrl WRITE setFileUrl NOTIFY configChanged)
    Q_PROPERTY(QString groupAddress READ groupAddress WRITE setGroupAddress NOTIFY configChanged)
    Q_PROPERTY(QString groupPort READ groupPort WRITE setGroupPort NOTIFY configChanged)
    Q_PROPERTY(int urlTypeIndex READ urlTypeIndex WRITE setUrlTypeIndex NOTIFY configChanged)
    Q_PROPERTY(bool addTestEnabled READ addTestEnabled WRITE setAddTestEnabled NOTIFY configChanged)
    Q_PROPERTY(bool outByOne READ outByOne WRITE setOutByOne NOTIFY configChanged)
    Q_PROPERTY(bool mergeChannels READ mergeChannels WRITE setMergeChannels NOTIFY configChanged)
    Q_PROPERTY(bool addLogoEnabled READ addLogoEnabled WRITE setAddLogoEnabled NOTIFY configChanged)
    Q_PROPERTY(bool addHdSuffix READ addHdSuffix WRITE setAddHdSuffix NOTIFY configChanged)
    Q_PROPERTY(QString maxExportResolution READ maxExportResolution WRITE setMaxExportResolution NOTIFY configChanged)
    Q_PROPERTY(QString fccUrl READ fccUrl WRITE setFccUrl NOTIFY configChanged)
    Q_PROPERTY(bool fccEnabled READ fccEnabled WRITE setFccEnabled NOTIFY configChanged)

    // ==================== Scanner QML属性 ====================
    Q_PROPERTY(QString scanUrl READ scanUrl WRITE setScanUrl NOTIFY configChanged)
    Q_PROPERTY(int threadMax READ threadMax WRITE setThreadMax NOTIFY configChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY configChanged)
    Q_PROPERTY(bool autoStep READ autoStep WRITE setAutoStep NOTIFY configChanged)
    Q_PROPERTY(bool useUrl READ useUrl WRITE setUseUrl NOTIFY configChanged)
    Q_PROPERTY(QString modelFilePath READ modelFilePath WRITE setModelFilePath NOTIFY configChanged)
    Q_PROPERTY(bool addTs READ addTs WRITE setAddTs NOTIFY configChanged)
    Q_PROPERTY(bool slowScan READ slowScan WRITE setSlowScan NOTIFY configChanged)

public:
    /** @brief 获取单例实例 */
    static AppConfig *instance();

    /** @brief 初始化配置文件路径
     *  @param path 配置文件路径 */
    void init(const QString &configPath);

    // ==================== 通用访问器 ====================
    QVariant value(const QString &key, const QVariant &defaultValue = {}) const;
    void setValue(const QString &key, const QVariant &value);

    // ==================== Manager: 流媒体URL设置 ====================
    QString httpUrl() const;
    void setHttpUrl(const QString &url);

    QString fileUrl() const;
    void setFileUrl(const QString &url);

    // ==================== Manager: 组播/扫描设置 ====================
    QString groupAddress() const;
    void setGroupAddress(const QString &addr);

    QString groupPort() const;
    void setGroupPort(const QString &port);

    int urlTypeIndex() const;
    void setUrlTypeIndex(int index);

    bool addTestEnabled() const;
    void setAddTestEnabled(bool enabled);

    // ==================== Manager: 导出设置 ====================
    bool outByOne() const;
    void setOutByOne(bool enabled);

    bool mergeChannels() const;
    void setMergeChannels(bool enabled);

    bool addLogoEnabled() const;
    void setAddLogoEnabled(bool enabled);

    bool addHdSuffix() const;
    void setAddHdSuffix(bool enabled);

    QString maxExportResolution() const;
    void setMaxExportResolution(const QString &resolution);

    // ==================== Manager: FCC设置 ====================
    QString fccUrl() const;
    void setFccUrl(const QString &url);

    bool fccEnabled() const;
    void setFccEnabled(bool enabled);

    // ==================== Manager: 分组选择 ====================
    QStringList selectedGroups() const;
    void setSelectedGroups(const QStringList &groups);

    // ==================== Scanner: 扫描设置 ====================
    QString scanUrl() const;
    void setScanUrl(const QString &url);

    int threadMax() const;
    void setThreadMax(int max);

    int timeout() const;
    void setTimeout(int ms);

    bool autoStep() const;
    void setAutoStep(bool enabled);

    bool useUrl() const;
    void setUseUrl(bool enabled);

    QString modelFilePath() const;
    void setModelFilePath(const QString &path);

    bool addTs() const;
    void setAddTs(bool enabled);

    bool slowScan() const;
    void setSlowScan(bool enabled);

signals:
    void configChanged();

private:
    explicit AppConfig(QObject *parent = nullptr);

    QSettings *m_settings = nullptr;
    static AppConfig *s_instance;
};

} // namespace Iptv::Core

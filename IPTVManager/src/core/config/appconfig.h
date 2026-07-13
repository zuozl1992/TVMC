#pragma once

#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>

namespace Iptv::Core {

/**
 * @brief 应用配置管理器（单例）
 *        管理所有持久化配置项的读写操作
 */
class AppConfig : public QObject
{
    Q_OBJECT

    //QML可访问的属性
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
    Q_PROPERTY(QString fccUrl READ fccUrl WRITE setFccUrl NOTIFY configChanged)
    Q_PROPERTY(bool fccEnabled READ fccEnabled WRITE setFccEnabled NOTIFY configChanged)

public:
    /** @brief 获取单例实例 */
    static AppConfig *instance();

    /** @brief 初始化配置文件路径
     *  @param path 配置文件路径 */
    void init(const QString &configPath);

    // ==================== 通用访问器 ====================
    QVariant value(const QString &key, const QVariant &defaultValue = {}) const;
    void setValue(const QString &key, const QVariant &value);

    // ==================== 流媒体URL设置 ====================
    /** @brief 获取/设置HTTP流媒体服务器地址 */
    QString httpUrl() const;
    void setHttpUrl(const QString &url);

    /** @brief 获取/设置文件服务器地址 */
    QString fileUrl() const;
    void setFileUrl(const QString &url);

    // ==================== 组播/扫描设置 ====================
    /** @brief 获取/设置组播地址模板 */
    QString groupAddress() const;
    void setGroupAddress(const QString &addr);

    /** @brief 获取/设置组播端口 */
    QString groupPort() const;
    void setGroupPort(const QString &port);

    /** @brief 获取/设置协议类型索引 */
    int urlTypeIndex() const;
    void setUrlTypeIndex(int index);

    /** @brief 获取/设置是否包含测试频道 */
    bool addTestEnabled() const;
    void setAddTestEnabled(bool enabled);

    // ==================== 导出设置 ====================
    /** @brief 获取/设置是否按分组单独导出 */
    bool outByOne() const;
    void setOutByOne(bool enabled);

    /** @brief 获取/设置是否合并相同频道 */
    bool mergeChannels() const;
    void setMergeChannels(bool enabled);

    /** @brief 获取/设置是否添加台标 */
    bool addLogoEnabled() const;
    void setAddLogoEnabled(bool enabled);

    /** @brief 获取/设置是否添加高清后缀 */
    bool addHdSuffix() const;
    void setAddHdSuffix(bool enabled);

    // ==================== FCC设置 ====================
    /** @brief 获取/设置FCC服务器地址 */
    QString fccUrl() const;
    void setFccUrl(const QString &url);

    /** @brief 获取/设置是否启用FCC */
    bool fccEnabled() const;
    void setFccEnabled(bool enabled);

    // ==================== 分组选择 ====================
    /** @brief 获取/设置选中的频道分组 */
    QStringList selectedGroups() const;
    void setSelectedGroups(const QStringList &groups);

signals:
    /** @brief 配置变更信号 */
    void configChanged();

private:
    explicit AppConfig(QObject *parent = nullptr);

    QSettings *m_settings = nullptr;    ///< QSettings实例
    static AppConfig *s_instance;       ///< 单例指针
};

} // namespace Iptv::Core

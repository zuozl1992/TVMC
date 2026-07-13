#pragma once

#include <QObject>
#include <QString>
#include <QHash>

namespace Iptv::Network {
class HttpClient;
}

namespace Iptv::Logic {

/**
 * @brief 频道指南数据管理器
 *        从网络下载EPG指南数据，解析频道ID映射
 */
class GuideManager : public QObject
{
    Q_OBJECT

public:
    explicit GuideManager(Network::HttpClient *httpClient, QObject *parent = nullptr);

    /** @brief 初始化（下载并解析指南数据）
     *  @param configDir 配置文件所在目录 */
    void init(const QString &configDir);

    /** @brief 根据频道名称查找频道ID
     *  @param name 频道名称
     *  @return 频道ID，未找到返回-1 */
    int findChannelId(const QString &name) const;

    /** @brief 是否已加载指南数据 */
    bool isLoaded() const;

private:
    /** @brief 下载指南数据
     *  @param configDir 配置文件所在目录
     *  @return 是否下载成功或已存在 */
    bool downloadGuide(const QString &configDir);

    /** @brief 解析指南数据
     *  @param filePath 指南文件路径 */
    void parseGuide(const QString &filePath);

    Network::HttpClient *m_httpClient;      ///< HTTP客户端
    QHash<QString, int> m_channelIdMap;     ///< 频道名称 -> 频道ID映射
    bool m_loaded = false;                  ///< 是否已加载指南数据
};

} // namespace Iptv::Logic

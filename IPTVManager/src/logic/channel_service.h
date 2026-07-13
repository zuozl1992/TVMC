#pragma once

#include <QObject>
#include <QJsonArray>
#include <QString>
#include <QHash>

namespace Iptv::Database {
class ChannelRepository;
class SourceRepository;
struct TvSource;
}

namespace Iptv::Export {
struct ExportOptions;
}

namespace Iptv::Logic {

class GuideManager;

/**
 * @brief 频道业务服务类
 *        封装频道和信号源的增删改查及导入导出逻辑
 */
class ChannelService : public QObject
{
    Q_OBJECT

public:
    explicit ChannelService(Database::ChannelRepository *channelRepo,
                            Database::SourceRepository *sourceRepo,
                            GuideManager *guideManager = nullptr,
                            QObject *parent = nullptr);

    /** @brief 添加频道
     *  @param name 频道名称
     *  @return 新频道的ID，失败返回-1 */
    int addChannel(const QString &name);

    /** @brief 获取或创建频道（幂等操作）
     *  @param name 频道名称
     *  @return 频道ID */
    int getOrCreateChannel(const QString &name);

    /** @brief 添加信号源
     *  @param tvId   关联的频道ID
     *  @param source 信号源信息
     *  @return 是否添加成功 */
    bool addSource(int tvId, const Database::TvSource &source);

    /** @brief 删除指定IP的信号源
     *  @param ip 信号源IP地址
     *  @return 是否删除成功 */
    bool removeSource(const QString &ip);

    /** @brief 删除所有信号源 */
    bool removeAllSources();

    /** @brief 删除所有频道 */
    bool removeAllChannels();

    /** @brief 从文件导入频道数据（根据扩展名自动识别格式）
     *  @param filePath 文件路径
     *  @return 导入的记录数，失败返回-1 */
    int importFromFile(const QString &filePath);

    /** @brief 从MC文件导入频道数据（自定义格式）
     *  @param filePath MC文件路径
     *  @return 导入的记录数，失败返回-1 */
    int importFromMc(const QString &filePath);

    /** @brief 从M3U文件导入频道数据
     *  @param filePath M3U文件路径
     *  @return 导入的记录数，失败返回-1 */
    int importFromM3u(const QString &filePath);

    /** @brief 从TXT文件导入频道数据（播放列表格式）
     *  @param filePath TXT文件路径
     *  @return 导入的记录数，失败返回-1 */
    int importFromTxt(const QString &filePath);

    /** @brief 构建指定分组的频道列表
     *  @param group 分组名称
     *  @param opts  导出选项
     *  @return 频道数据JSON数组 */
    QJsonArray buildChannelList(const QString &group,
                                const Export::ExportOptions &opts);

    /** @brief 构建按分组组织的频道列表
     *  @param groups 分组名称列表
     *  @return 分组名到频道数据的映射 */
    QHash<QString, QJsonArray> buildGroupedLists(const QStringList &groups);

private:
    Database::ChannelRepository *m_channelRepo; ///< 频道数据仓库
    Database::SourceRepository *m_sourceRepo;   ///< 信号源数据仓库
    GuideManager *m_guideManager;               ///< 频道指南管理器
};

} // namespace Iptv::Logic

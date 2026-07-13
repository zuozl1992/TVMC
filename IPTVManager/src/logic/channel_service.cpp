#include "channel_service.h"
#include "guide_manager.h"
#include "database/channel_repository.h"
#include "database/source_repository.h"
#include "database/types.h"
#include "export/exporter.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QRegularExpression>

namespace Iptv::Logic {

//清理频道名称，移除分辨率/质量后缀
static QString cleanChannelName(const QString &name)
{
    QString result = name.trimmed();
    //移除常见的分辨率/质量后缀
    QStringList suffixes = {" 4K", " 8K", " HD", " SD", " FHD", " UHD", " HDR", " SDR"};
    for (const QString &suffix : suffixes) {
        if (result.endsWith(suffix, Qt::CaseInsensitive)) {
            result = result.left(result.length() - suffix.length()).trimmed();
        }
    }
    return result;
}

ChannelService::ChannelService(Database::ChannelRepository *channelRepo,
                               Database::SourceRepository *sourceRepo,
                               GuideManager *guideManager,
                               QObject *parent)
    : QObject(parent)
    , m_channelRepo(channelRepo)
    , m_sourceRepo(sourceRepo)
    , m_guideManager(guideManager)
{
}

int ChannelService::addChannel(const QString &name)
{
    m_channelRepo->insertChannel(name);
    return m_channelRepo->findIdByName(name);
}

int ChannelService::getOrCreateChannel(const QString &name)
{
    int id = m_channelRepo->findIdByName(name);
    if (id < 0) {
        id = addChannel(name);
        //从指南数据中查找并更新频道ID
        if (id >= 0 && m_guideManager && m_guideManager->isLoaded()) {
            int guideId = m_guideManager->findChannelId(name);
            if (guideId > 0) {
                Database::TvChannel channel = m_channelRepo->findById(id);
                channel.channelId = guideId;
                m_channelRepo->updateChannel(id, channel);
                qInfo() << "Channel ID synced from guide:" << name << "->" << guideId;
            }
        }
    }
    return id;
}

bool ChannelService::addSource(int tvId, const Database::TvSource &source)
{
    Database::TvSource src = source;
    src.tvId = tvId;
    return m_sourceRepo->upsertSource(src);
}

bool ChannelService::removeSource(const QString &ip)
{
    return m_sourceRepo->deleteByIp(ip);
}

bool ChannelService::removeAllSources()
{
    return m_sourceRepo->deleteAll();
}

bool ChannelService::removeAllChannels()
{
    return m_channelRepo->deleteAll();
}

int ChannelService::importFromFile(const QString &filePath)
{
    //根据文件扩展名选择导入方式
    QString ext = QFileInfo(filePath).suffix().toLower();
    if (ext == "mc") {
        return importFromMc(filePath);
    } else if (ext == "m3u") {
        return importFromM3u(filePath);
    } else if (ext == "txt") {
        return importFromTxt(filePath);
    }
    qWarning() << "Unsupported file format:" << ext;
    return -1;
}

int ChannelService::importFromMc(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << filePath;
        return -1;
    }
    
    int count = 0;
    int errCount = 0;
    
    //逐行解析MC文件
    while (!file.atEnd()) {
        QByteArray data = file.readLine();
        if (data.isEmpty())
            break;
        
        //移除换行符
        if (data.indexOf('\n') >= 0)
            data.removeLast();
        if (data.indexOf('\r') >= 0)
            data.removeLast();
        
        QString line = QString::fromUtf8(data).trimmed();
        if (line.isEmpty())
            continue;
        
        //跳过无逗号的行
        if (!line.contains(","))
            continue;
        
        //解析行格式："频道名 类型 分辨率(帧率),URL"
        QStringList parts = line.split(",");
        if (parts.length() != 2) {
            errCount++;
            continue;
        }
        
        QString namePart = parts.at(0).trimmed();
        QString url = parts.at(1).trimmed();
        
        //忽略URL中?以后的数据
        int questionMark = url.indexOf("?");
        if (questionMark >= 0) {
            url = url.left(questionMark);
        }
        
        //从URL中提取IP和端口
        int i = url.indexOf("/udp/");
        int j = url.indexOf("/rtp/");
        if (i < 0 && j < 0) {
            errCount++;
            continue;
        }
        
        int start = (i >= 0 ? i : j) + 5;
        int end = url.indexOf(":", start);
        if (end < 0) {
            errCount++;
            continue;
        }
        QString ip = url.mid(start, end - start);
        int port = url.mid(end + 1).toInt();
        
        //解析名称部分："名称 类型 分辨率(帧率)"
        QStringList nameParts = namePart.split(" ");
        if (nameParts.length() != 3) {
            errCount++;
            continue;
        }
        
        QString name = nameParts.at(0);
        QString typeStr = nameParts.at(1);
        QString resFpsStr = nameParts.at(2);
        
        //解析类型
        int type = typeStr == "SD" ? 1 : typeStr == "HD" ? 2 : typeStr == "4K" ? 3 : typeStr == "8K" ? 4 : 0;
        
        //解析分辨率和帧率："1920x1080(25)"
        QStringList resFpsParts = resFpsStr.split("(");
        if (resFpsParts.length() != 2) {
            errCount++;
            continue;
        }
        
        QString resPart = resFpsParts.at(0);
        QString fpsStr = resFpsParts.at(1).split(")").at(0);
        
        QStringList resParts = resPart.split("x");
        if (resParts.length() != 2) {
            errCount++;
            continue;
        }
        
        int width = resParts.at(0).toInt();
        int height = resParts.at(1).toInt();
        int fps = fpsStr.toInt();
        
        //获取或创建频道（清理名称中的分辨率后缀）
        QString channelName = cleanChannelName(name);
        int channelId = getOrCreateChannel(channelName);
        if (channelId < 0) {
            qWarning() << "Failed to create channel:" << channelName;
            errCount++;
            continue;
        }
        
        //创建信号源
        Database::TvSource source;
        source.tvId = channelId;
        source.ip = ip;
        source.port = port;
        source.width = width;
        source.height = height;
        source.fps = fps;
        source.type = type;
        
        if (addSource(channelId, source)) {
            count++;
        } else {
            errCount++;
        }
    }
    
    file.close();
    qInfo() << "MC import completed: success=" << count << " failed=" << errCount;
    return count;
}

int ChannelService::importFromM3u(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << filePath;
        return -1;
    }
    
    int count = 0;
    int errCount = 0;
    QString currentName;
    QString currentGroup;
    
    //逐行解析M3U文件
    while (!file.atEnd()) {
        QByteArray data = file.readLine();
        if (data.isEmpty())
            break;
        
        QString line = QString::fromUtf8(data).trimmed();
        if (line.isEmpty())
            continue;
        
        //跳过M3U头
        if (line.startsWith("#EXTM3U"))
            continue;
        
        //解析EXTINF行
        if (line.startsWith("#EXTINF:")) {
            //提取group-title
            QRegularExpression groupRe("group-title=\"([^\"]+)\"");
            QRegularExpressionMatch groupMatch = groupRe.match(line);
            if (groupMatch.hasMatch()) {
                currentGroup = groupMatch.captured(1);
            }
            
            //提取频道名称（逗号后面的部分）
            int commaIdx = line.lastIndexOf(",");
            if (commaIdx >= 0) {
                currentName = line.mid(commaIdx + 1).trimmed();
            }
            continue;
        }
        
        //跳过注释行
        if (line.startsWith("#"))
            continue;
        
        //当前行是URL
        QString url = line;
        
        //忽略URL中?以后的数据
        int questionMark = url.indexOf("?");
        if (questionMark >= 0) {
            url = url.left(questionMark);
        }
        
        //从URL中提取IP和端口
        int i = url.indexOf("/udp/");
        int j = url.indexOf("/rtp/");
        if (i < 0 && j < 0) {
            errCount++;
            continue;
        }
        
        int start = (i >= 0 ? i : j) + 5;
        int end = url.indexOf(":", start);
        if (end < 0) {
            errCount++;
            continue;
        }
        QString ip = url.mid(start, end - start);
        int port = url.mid(end + 1).toInt();
        
        if (currentName.isEmpty()) {
            currentName = ip;
        }
        
        //获取或创建频道（清理名称中的分辨率后缀）
        QString channelName = cleanChannelName(currentName);
        int channelId = getOrCreateChannel(channelName);
        if (channelId < 0) {
            qWarning() << "Failed to create channel:" << channelName;
            errCount++;
            continue;
        }
        
        //创建信号源
        Database::TvSource source;
        source.tvId = channelId;
        source.ip = ip;
        source.port = port;
        
        if (addSource(channelId, source)) {
            count++;
        } else {
            errCount++;
        }
        
        //重置当前频道信息
        currentName.clear();
        currentGroup.clear();
    }
    
    file.close();
    qInfo() << "M3U import completed: success=" << count << " failed=" << errCount;
    return count;
}

int ChannelService::importFromTxt(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << filePath;
        return -1;
    }
    
    int count = 0;
    int errCount = 0;
    QString currentGroup;
    
    //逐行解析TXT播放列表文件
    //格式：GroupName,#genre#\nChannelName,URL
    while (!file.atEnd()) {
        QByteArray data = file.readLine();
        if (data.isEmpty())
            break;
        
        QString line = QString::fromUtf8(data).trimmed();
        if (line.isEmpty())
            continue;
        
        //跳过无逗号的行
        if (!line.contains(","))
            continue;
        
        QStringList parts = line.split(",");
        if (parts.length() != 2) {
            errCount++;
            continue;
        }
        
        QString namePart = parts.at(0).trimmed();
        QString url = parts.at(1).trimmed();
        
        //分组标题行：GroupName,#genre#
        if (url == "#genre#") {
            currentGroup = namePart;
            continue;
        }
        
        //忽略URL中?以后的数据
        int questionMark = url.indexOf("?");
        if (questionMark >= 0) {
            url = url.left(questionMark);
        }
        
        //从URL中提取IP和端口
        int i = url.indexOf("/udp/");
        int j = url.indexOf("/rtp/");
        if (i < 0 && j < 0) {
            errCount++;
            continue;
        }
        
        int start = (i >= 0 ? i : j) + 5;
        int end = url.indexOf(":", start);
        if (end < 0) {
            errCount++;
            continue;
        }
        QString ip = url.mid(start, end - start);
        int port = url.mid(end + 1).toInt();
        
        //获取或创建频道（清理名称中的分辨率后缀）
        QString channelName = cleanChannelName(namePart);
        int channelId = getOrCreateChannel(channelName);
        if (channelId < 0) {
            qWarning() << "Failed to create channel:" << channelName;
            errCount++;
            continue;
        }
        
        //创建信号源
        Database::TvSource source;
        source.tvId = channelId;
        source.ip = ip;
        source.port = port;
        
        if (addSource(channelId, source)) {
            count++;
        } else {
            errCount++;
        }
    }
    
    file.close();
    qInfo() << "TXT import completed: success=" << count << " failed=" << errCount;
    return count;
}

QJsonArray ChannelService::buildChannelList(const QString &group,
                                             const Export::ExportOptions &opts)
{
    Q_UNUSED(opts)
    return m_sourceRepo->getTvListByGroup(group, true);
}

QHash<QString, QJsonArray> ChannelService::buildGroupedLists(const QStringList &groups)
{
    QHash<QString, QJsonArray> result;
    
    for (const QString &group : groups) {
        QJsonArray list = m_sourceRepo->getTvListByGroup(group, false);
        if (!list.isEmpty()) {
            result[group] = list;
        }
    }
    
    return result;
}

} // namespace Iptv::Logic

#include "guide_manager.h"
#include "network/httpclient.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QNetworkReply>
#include <QEventLoop>
#include <QXmlStreamReader>
#include <QRegularExpression>
#include <QProcess>
#include <QDebug>

namespace Iptv::Logic {

GuideManager::GuideManager(Network::HttpClient *httpClient, QObject *parent)
    : QObject(parent)
    , m_httpClient(httpClient)
{
}

void GuideManager::init(const QString &configDir)
{
    if (downloadGuide(configDir)) {
        QString filePath = configDir + "/epg_guide.xml.gz";
        if (!QFile::exists(filePath)) {
            filePath = configDir + "/epg_guide.xml";
        }
        parseGuide(filePath);
    }
}

int GuideManager::findChannelId(const QString &name) const
{
    return m_channelIdMap.value(name, -1);
}

bool GuideManager::isLoaded() const
{
    return m_loaded;
}

bool GuideManager::downloadGuide(const QString &configDir)
{
    //检查是否已有今天的指南文件
    QString gzPath = configDir + "/epg_guide.xml.gz";
    QString xmlPath = configDir + "/epg_guide.xml";

    //检查.gz文件是否为今天
    if (QFile::exists(gzPath)) {
        QFileInfo info(gzPath);
        if (info.lastModified().date() == QDate::currentDate()) {
            qInfo() << "Guide file already exists for today:" << gzPath;
            return true;
        }
    }

    //检查.xml文件是否为今天
    if (QFile::exists(xmlPath)) {
        QFileInfo info(xmlPath);
        if (info.lastModified().date() == QDate::currentDate()) {
            qInfo() << "Guide file already exists for today:" << xmlPath;
            return true;
        }
    }

    //下载指南数据
    qInfo() << "Downloading guide data from http://e.erw.cc/e.xml.gz...";
    if (!m_httpClient) {
        qWarning() << "HttpClient not available for guide download";
        return false;
    }

    QNetworkReply *reply = m_httpClient->get(QUrl("http://e.erw.cc/e.xml.gz"));
    if (!reply) {
        qWarning() << "Failed to start guide download";
        return false;
    }

    //同步等待下载完成
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Guide download failed:" << reply->errorString();
        reply->deleteLater();
        return false;
    }

    //保存到文件
    QDir().mkpath(configDir);
    QFile file(gzPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to save guide file:" << gzPath;
        reply->deleteLater();
        return false;
    }

    file.write(reply->readAll());
    file.close();
    reply->deleteLater();

    qInfo() << "Guide downloaded to:" << gzPath;

    //解压.gz文件
    QProcess gzip;
    gzip.start("gzip", {"-d", "-f", gzPath});
    gzip.waitForFinished(10000);

    if (!QFile::exists(xmlPath)) {
        qWarning() << "Failed to decompress guide file";
        return false;
    }

    qInfo() << "Guide decompressed to:" << xmlPath;
    return true;
}

void GuideManager::parseGuide(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open guide file:" << filePath;
        return;
    }

    qInfo() << "Parsing guide file:" << filePath;
    m_channelIdMap.clear();

    //解析XML格式的EPG指南
    QXmlStreamReader xml(&file);
    QString currentChannelId;
    QString currentDisplayName;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "channel") {
                //频道元素开始
                currentChannelId = xml.attributes().value("id").toString();
                currentDisplayName.clear();
            } else if (xml.name() == "display-name") {
                //频道显示名称
                currentDisplayName = xml.readElementText().trimmed();
            }
        } else if (xml.isEndElement()) {
            if (xml.name() == "channel") {
                //频道元素结束，保存映射
                if (!currentChannelId.isEmpty() && !currentDisplayName.isEmpty()) {
                    //尝试从channelId中提取数字ID
                    int id = currentChannelId.toInt();
                    if (id > 0) {
                        m_channelIdMap[currentDisplayName] = id;
                    }
                }
                currentChannelId.clear();
                currentDisplayName.clear();
            }
        }
    }

    if (xml.hasError()) {
        qWarning() << "XML parsing error:" << xml.errorString();
    }

    m_loaded = !m_channelIdMap.isEmpty();
    qInfo() << "Guide parsed:" << m_channelIdMap.size() << "channels loaded";
}

} // namespace Iptv::Logic

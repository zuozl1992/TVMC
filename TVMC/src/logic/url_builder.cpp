#include "url_builder.h"

#include <QFile>
#include <QRegularExpression>
#include <QDebug>

namespace Iptv::Logic {

QStringList UrlBuilder::expand(const QString &templ)
{
    QString temp = templ;
    //限制最多3层花括号嵌套
    if (temp.split("{").length() > 4)
        return {};

    int sIndex = temp.indexOf("{");
    int eIndex = temp.indexOf("}");
    if (sIndex < 0 && eIndex < 0) {
        //无花括号：直接作为单个模式展开
        QStringList expanded = expandRange(temp);
        return expanded;
    } else if (sIndex < 0 || eIndex < 0 || eIndex <= sIndex) {
        return {};
    }

    //展开花括号内的范围表达式
    QStringList strList = expandRange(temp.mid(sIndex + 1, eIndex - sIndex - 1));
    QString a = temp.mid(0, sIndex);
    QString b = temp.mid(eIndex + 1);

    QStringList result;
    if (b.indexOf("{") >= 0) {
        //递归展开后续的花括号表达式
        QStringList tre = expand(b);
        if (tre.isEmpty())
            return {};
        for (int i = 0; i < strList.length(); i++) {
            for (int j = 0; j < tre.length(); j++) {
                result.append(QString("%1%2%3")
                    .arg(a)
                    .arg(strList.at(i))
                    .arg(tre.at(j)));
            }
        }
    } else {
        for (int i = 0; i < strList.length(); i++) {
            result.append(QString("%1%2%3")
                .arg(a)
                .arg(strList.at(i))
                .arg(b));
        }
    }
    return result;
}

QStringList UrlBuilder::buildScanIps(const QString &groupAddress,
                                     const QString &port,
                                     const QStringList &existingIps)
{
    //展开组播地址模板获取IP前缀
    QStringList prefixes = expand(groupAddress);
    QStringList result;

    //对每个前缀遍历1-255生成完整IP
    for (const QString &prefix : prefixes) {
        for (int j = 1; j <= 255; j++) {
            QString ip = prefix + QString(".%1").arg(j);
            //过滤数据库中已存在的IP
            if (existingIps.contains(ip))
                continue;
            if (port.isEmpty())
                result << ip;
            else
                result << QString("%1:{%2}").arg(ip).arg(port);
        }
    }

    return result;
}

QStringList UrlBuilder::buildTestUrls(const QString &port,
                                      const QStringList &existingIps)
{
    Q_UNUSED(port)
    Q_UNUSED(existingIps)
    return {};
}

QStringList UrlBuilder::expandFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return {};

    //逐行读取模板文件并展开
    QStringList result;
    while (true) {
        QByteArray data = file.readLine();
        if (data.isEmpty())
            break;
        if (data.lastIndexOf('\n') >= 0)
            data.removeLast();
        if (data.lastIndexOf('\r') >= 0)
            data.removeLast();
        QStringList expanded = expand(QString::fromLocal8Bit(data));
        if (expanded.isEmpty())
            return {};
        result.append(expanded);
    }
    return result;
}

QString UrlBuilder::getIpFromUrl(const QString &url)
{
    //处理udp://和rtp://协议
    if (url.startsWith("udp://") || url.startsWith("rtp://")) {
        int start = 6;
        int end = url.indexOf(":", start);
        if (end > start) {
            return url.mid(start, end - start);
        }
    }

    //处理http://.../udp/...格式
    int i1 = url.indexOf("/udp/");
    int i2 = url.indexOf("/rtp/");
    if (i1 >= 0 || i2 >= 0) {
        int start = (i1 < 0 ? i2 : i1) + 5;
        int end = url.indexOf(":", start);
        if (end > start) {
            return url.mid(start, end - start);
        }
    }

    //回退：尝试从"://"后提取
    int schemeEnd = url.indexOf("://");
    if (schemeEnd >= 0) {
        int start = schemeEnd + 3;
        int end = url.indexOf(":", start);
        if (end > start) {
            return url.mid(start, end - start);
        }
    }

    return url;
}

QStringList UrlBuilder::expandRange(const QString &pattern)
{
    QStringList result;

    //检查是否为#分隔的多值表达式（可包含范围）
    if (pattern.contains("#")) {
        QStringList parts = pattern.split("#");
        for (const QString &part : parts) {
            if (part.startsWith("[")) {
                //混合记法中的范围模式
                QString r = part.mid(1, part.length() - 2);
                QStringList rl = r.remove(QRegularExpression("\\s")).split("-");
                if (rl.length() != 2)
                    continue;
                int s = rl.at(0).toInt();
                int e = rl.at(1).toInt();
                int len = rl.at(0).length();
                if (s >= e || s <= 0 || e <= 0)
                    continue;
                for (int j = s; j <= e; j++)
                    result.append(QString("%1").arg(j, len, 10, QChar('0')));
            } else {
                result.append(part);
            }
        }
        return result;
    }

    //检查是否为范围模式：[start-end]
    if (pattern.startsWith("[")) {
        QString r = pattern.mid(1, pattern.length() - 2);
        QStringList rl = r.remove(QRegularExpression("\\s")).split("-");
        if (rl.length() != 2)
            return {pattern};
        int s = rl.at(0).toInt();
        int e = rl.at(1).toInt();
        int len = rl.at(0).length();
        if (s >= e || s <= 0 || e <= 0)
            return {pattern};
        for (int j = s; j <= e; j++)
            result.append(QString("%1").arg(j, len, 10, QChar('0')));
        return result;
    }

    //单个值直接返回
    result << pattern;
    return result;
}

} // namespace Iptv::Logic

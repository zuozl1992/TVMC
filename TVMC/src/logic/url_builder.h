#pragma once

#include <QString>
#include <QStringList>

namespace Iptv::Logic {

/**
 * @brief URL构建工具类（合并版）
 *        合并Manager和Scanner的URL构建功能
 */
class UrlBuilder
{
public:
    /** @brief 展开地址模板中的花括号表达式
     *  @param templ 包含{...}的模板字符串，支持#分隔和[start-end]范围
     *  @return 展开后的字符串列表 */
    static QStringList expand(const QString &templ);

    /** @brief 构建待扫描的IP地址列表（Manager专用）
     *  @param groupAddress 组播地址模板
     *  @param port         端口号
     *  @param existingIps  数据库中已存在的IP列表
     *  @return 格式为ip:{port}的扫描地址列表 */
    static QStringList buildScanIps(const QString &groupAddress,
                                    const QString &port,
                                    const QStringList &existingIps);

    /** @brief 构建测试频道URL列表（Manager专用）
     *  @param port        端口号
     *  @param existingIps 已存在的IP列表
     *  @return 测试频道URL列表 */
    static QStringList buildTestUrls(const QString &port,
                                     const QStringList &existingIps);

    /** @brief 从文件读取URL模板并展开（Scanner专用）
     *  @param filePath 模板文件路径（每行一个模板）
     *  @return 展开后的URL列表 */
    static QStringList expandFromFile(const QString &filePath);

    /** @brief 从URL中提取IP地址（Scanner专用）
     *  @param url 完整URL
     *  @return IP地址字符串 */
    static QString getIpFromUrl(const QString &url);

private:
    /** @brief 展开单个范围表达式 */
    static QStringList expandRange(const QString &pattern);
};

} // namespace Iptv::Logic

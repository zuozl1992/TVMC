#pragma once

#include <QString>
#include <QVariant>

namespace Iptv::Core {

/**
 * @brief 启动引导配置
 *        管理应用启动时的配置文件和数据库路径
 */
class Bootstrap
{
public:
    Bootstrap();

    /** @brief 获取配置文件路径 */
    QString configPath() const;

    /** @brief 获取数据库文件路径 */
    QString dbPath() const;

    /** @brief 获取引导文件路径 */
    QString bootstrapPath() const;

    /** @brief 设置配置文件路径 */
    void setConfigPath(const QString &path);

    /** @brief 设置数据库文件路径 */
    void setDbPath(const QString &path);

    /** @brief 检查配置是否有效（路径均已设置） */
    bool isValid() const;

    /** @brief 从引导文件加载配置 */
    bool load();

    /** @brief 保存配置到引导文件 */
    bool save() const;

private:
    QString m_configPath;       ///< 配置文件路径
    QString m_dbPath;           ///< 数据库文件路径
    QString m_bootstrapPath;    ///< 引导文件路径
};

} // namespace Iptv::Core

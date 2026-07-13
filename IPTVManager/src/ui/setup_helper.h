#pragma once

#include <QObject>
#include <QString>

namespace Iptv::Ui {

/**
 * @brief 配置助手类
 *        为QML提供保存bootstrap配置的功能
 */
class SetupHelper : public QObject
{
    Q_OBJECT

public:
    explicit SetupHelper(const QString &bootstrapPath, QObject *parent = nullptr);

    /** @brief 保存配置路径并退出应用
     *  @param configPath 配置文件路径
     *  @param dbPath     数据库文件路径 */
    Q_INVOKABLE void saveAndQuit(const QString &configPath, const QString &dbPath);

private:
    QString m_bootstrapPath;    ///< 引导文件路径
};

} // namespace Iptv::Ui

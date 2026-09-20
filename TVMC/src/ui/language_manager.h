#pragma once

#include <QObject>
#include <QTranslator>
#include <QGuiApplication>

namespace Iptv::Ui {

/**
 * @brief 语言管理器
 *        支持运行时切换界面语言
 */
class LanguageManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentLanguage READ currentLanguage NOTIFY languageChanged)

public:
    explicit LanguageManager(QGuiApplication *app, QObject *parent = nullptr);

    /** @brief 获取当前语言 */
    QString currentLanguage() const;

    /** @brief 切换语言
     *  @param locale 语言代码，如 "zh_CN", "en_US" */
    Q_INVOKABLE void switchLanguage(const QString &locale);

    /** @brief 获取可用语言列表 */
    Q_INVOKABLE QStringList availableLanguages() const;

signals:
    void languageChanged();

private:
    QGuiApplication *m_app;
    QTranslator *m_translator;
    QString m_currentLanguage;
};

} // namespace Iptv::Ui

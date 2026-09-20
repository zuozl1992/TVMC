#include "language_manager.h"
#include "core/config/appconfig.h"
#include <QDebug>
#include <QProcess>

namespace Iptv::Ui {

LanguageManager::LanguageManager(QGuiApplication *app, QObject *parent)
    : QObject(parent)
    , m_app(app)
    , m_translator(new QTranslator(this))
    , m_currentLanguage("zh_CN")
{
    //从配置加载语言设置
    Core::AppConfig *config = Core::AppConfig::instance();
    QString savedLang = config->value("set/language", "zh_CN").toString();
    if (!savedLang.isEmpty()) {
        m_currentLanguage = savedLang;
    }
}

QString LanguageManager::currentLanguage() const
{
    return m_currentLanguage;
}

void LanguageManager::switchLanguage(const QString &locale)
{
    if (m_currentLanguage == locale) return;

    //保存语言设置到配置
    Core::AppConfig *config = Core::AppConfig::instance();
    config->setValue("set/language", locale);
    m_currentLanguage = locale;

    qInfo() << "Language saved:" << locale << ", restarting application...";

    //重启应用
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

QStringList LanguageManager::availableLanguages() const
{
    return {"zh_CN", "en_US"};
}

} // namespace Iptv::Ui

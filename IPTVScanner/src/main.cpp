#include "core/config/appconfig.h"
#include "core/logging/logmanager.h"
#include "ui/scanner_backend.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QTranslator>
#include <QLocale>
#include <QStandardPaths>
#include <QDir>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("IPTVScanner");
    app.setApplicationVersion("2.1.0");
    app.setOrganizationName("IPTVScanner");

    //设置Material样式
    QQuickStyle::setStyle("Material");

    //加载翻译文件
    QTranslator *translator = new QTranslator(&app);
    bool loaded = false;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "IPTVScanner_" + QLocale(locale).name();
        if (translator->load(":/i18n/" + baseName)) {
            loaded = true;
            break;
        }
    }
    if (!loaded) {
        translator->load(":/i18n/IPTVScanner_zh_CN");
    }
    app.installTranslator(translator);

    //初始化日志系统
    Iptv::Core::LogManager::init();
    qInfo() << "Application starting...";

    //初始化配置文件
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configDir);
    QString configPath = configDir + "/IPTVScanner.ini";
    Iptv::Core::AppConfig::instance()->init(configPath);
    qInfo() << "Config loaded from:" << configPath;

    //创建QML桥接后端
    Iptv::Ui::ScannerBackend backend;

    //创建QML引擎
    QQmlApplicationEngine engine;

    //添加QML组件导入路径
    engine.addImportPath(":/qml/components");

    //注册后端到QML上下文
    engine.rootContext()->setContextProperty("backend", &backend);

    //加载主QML文件
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "Failed to load QML";
        return -1;
    }

    qInfo() << "Application started.";
    return app.exec();
}

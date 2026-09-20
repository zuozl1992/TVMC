#include "core/config/bootstrap.h"
#include "core/config/appconfig.h"
#include "core/logging/logmanager.h"
#include "ui/manager_backend.h"
#include "ui/scanner_backend.h"
#include "ui/language_manager.h"
#include "ui/frame_image_provider.h"
#include "ui/logo_image_provider.h"
#include "ui/setup_helper.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QTranslator>
#include <QLocale>
#include <QStandardPaths>
#include <QDebug>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setApplicationName("TVMC");
    app.setApplicationVersion("2.2.1");
    app.setOrganizationName("TVMC");

    //设置Material样式
    QQuickStyle::setStyle("Material");

    //初始化日志系统
    Iptv::Core::LogManager::init();
    qInfo() << "Application starting...";

    //加载引导配置
    Iptv::Core::Bootstrap bootstrap;
    QString configPath;
    QString dbPath;

    if (bootstrap.load()) {
        configPath = bootstrap.configPath();
        dbPath = bootstrap.dbPath();
        qInfo() << "Bootstrap loaded: config=" << configPath << " db=" << dbPath;

        //初始化配置管理器
        Iptv::Core::AppConfig::instance()->init(configPath);

        //创建语言管理器（会从配置加载语言设置）
        Iptv::Ui::LanguageManager languageManager(&app);

        //加载翻译（使用new保持生命周期）
        QTranslator *translator = new QTranslator(&app);
        if (translator->load(":/i18n/TVMC_" + languageManager.currentLanguage())) {
            app.installTranslator(translator);
        }

        //创建图像提供者
        Iptv::Ui::FrameImageProvider *frameProvider = new Iptv::Ui::FrameImageProvider();
        Iptv::Ui::LogoImageProvider *logoProvider = new Iptv::Ui::LogoImageProvider();

        //创建Manager后端
        Iptv::Ui::ManagerBackend backend;
        backend.setFrameImageProvider(frameProvider);
        backend.setLogoImageProvider(logoProvider);
        backend.setBootstrapPath(bootstrap.bootstrapPath());

        //创建Scanner后端
        Iptv::Ui::ScannerBackend scannerBackend;

        //创建QML引擎
        QQmlApplicationEngine engine;

        //添加QML组件导入路径
        engine.addImportPath(":/qml/components");
        engine.addImportPath(":/qml/tabs");
        engine.addImportPath(":/qml/dialogs");

        //注册图像提供者
        engine.addImageProvider("frame", frameProvider);
        engine.addImageProvider("logo", logoProvider);

        //注册后端到QML上下文
        engine.rootContext()->setContextProperty("backend", &backend);
        engine.rootContext()->setContextProperty("scannerBackend", &scannerBackend);
        engine.rootContext()->setContextProperty("languageManager", &languageManager);
        engine.rootContext()->setContextProperty("configPath", configPath);
        engine.rootContext()->setContextProperty("dbPath", dbPath);

        //加载主QML文件
        engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

        if (engine.rootObjects().isEmpty()) {
            qCritical() << "Failed to load QML";
            return -1;
        }

        qInfo() << "Application started with QML.";
        return app.exec();

    } else {
        //无有效引导配置，显示配置页面
        qInfo() << "No valid bootstrap found, showing setup dialog";

        //创建配置助手
        Iptv::Ui::SetupHelper setupHelper(bootstrap.bootstrapPath());

        //创建QML引擎
        QQmlApplicationEngine engine;

        //添加QML组件导入路径
        engine.addImportPath(":/qml/components");
        engine.addImportPath(":/qml/dialogs");

        //注册配置助手到QML上下文
        engine.rootContext()->setContextProperty("setupHelper", &setupHelper);

        //加载配置页面QML
        engine.load(QUrl(QStringLiteral("qrc:/qml/dialogs/DbSetupDialog.qml")));

        if (engine.rootObjects().isEmpty()) {
            qCritical() << "Failed to load setup QML";
            return -1;
        }

        qInfo() << "Setup dialog shown.";
        return app.exec();
    }
}

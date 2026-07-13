#pragma once

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QPixmap>
#include <QString>
#include <QStringList>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include "models/check_list_model.h"
#include "database/sql_query_model_qml.h"
#include "frame_image_provider.h"
#include "logo_image_provider.h"

namespace Iptv::Core {
class AppConfig;
}

namespace Iptv::Database {
class DatabaseManager;
class ChannelRepository;
class SourceRepository;
}

namespace Iptv::Network {
class HttpClient;
class LogoFetcher;
}

namespace Iptv::Multimedia {
class StreamProbe;
struct StreamInfo;
}

namespace Iptv::Export {
class XlsxExporter;
}

namespace Iptv::Logic {
class ChannelService;
class CheckService;
class GuideManager;
}

namespace Iptv::Ui {

/**
 * @brief IPTVManager QML桥接类
 *        将所有业务逻辑暴露给QML界面
 */
class ManagerBackend : public QObject
{
    Q_OBJECT

    //配置属性
    Q_PROPERTY(QString httpUrl READ httpUrl WRITE setHttpUrl NOTIFY configChanged)
    Q_PROPERTY(QString fileUrl READ fileUrl WRITE setFileUrl NOTIFY configChanged)
    Q_PROPERTY(QString groupAddress READ groupAddress WRITE setGroupAddress NOTIFY configChanged)
    Q_PROPERTY(QString groupPort READ groupPort WRITE setGroupPort NOTIFY configChanged)
    Q_PROPERTY(int urlTypeIndex READ urlTypeIndex WRITE setUrlTypeIndex NOTIFY configChanged)
    Q_PROPERTY(bool addTestEnabled READ addTestEnabled WRITE setAddTestEnabled NOTIFY configChanged)
    Q_PROPERTY(bool mergeChannels READ mergeChannels WRITE setMergeChannels NOTIFY configChanged)
    Q_PROPERTY(bool addLogoEnabled READ addLogoEnabled WRITE setAddLogoEnabled NOTIFY configChanged)
    Q_PROPERTY(bool addHdSuffix READ addHdSuffix WRITE setAddHdSuffix NOTIFY configChanged)
    Q_PROPERTY(QString fccUrl READ fccUrl WRITE setFccUrl NOTIFY configChanged)
    Q_PROPERTY(bool fccEnabled READ fccEnabled WRITE setFccEnabled NOTIFY configChanged)
    Q_PROPERTY(QStringList selectedGroups READ selectedGroups WRITE setSelectedGroups NOTIFY configChanged)
    Q_PROPERTY(QString importFilePath READ importFilePath WRITE setImportFilePath NOTIFY importFilePathChanged)

    //Tab0 - 浏览模型
    Q_PROPERTY(QSqlQueryModel* browseModel READ browseModel CONSTANT)

    //Tab1 - 编辑模型
    Q_PROPERTY(QSqlTableModel* channelModel READ channelModel CONSTANT)

    //Tab2 - 检测状态
    Q_PROPERTY(int checkIndex READ checkIndex WRITE setCheckIndex NOTIFY checkIndexChanged)
    Q_PROPERTY(int checkTotal READ checkTotal NOTIFY checkTotalChanged)
    Q_PROPERTY(bool checking READ isChecking NOTIFY checkingChanged)
    Q_PROPERTY(int checkProgress READ checkProgress NOTIFY checkProgressChanged)
    Q_PROPERTY(QString checkUrl READ checkUrl NOTIFY checkUrlChanged)
    Q_PROPERTY(QString checkName READ checkName WRITE setCheckName NOTIFY checkNameChanged)
    Q_PROPERTY(int checkWidth READ checkWidth NOTIFY checkWidthChanged)
    Q_PROPERTY(int checkHeight READ checkHeight NOTIFY checkHeightChanged)
    Q_PROPERTY(int checkFps READ checkFps NOTIFY checkFpsChanged)
    Q_PROPERTY(int checkSourceType READ checkSourceType WRITE setCheckSourceType NOTIFY checkSourceTypeChanged)
    Q_PROPERTY(QString checkNotes READ checkNotes WRITE setCheckNotes NOTIFY checkNotesChanged)
    Q_PROPERTY(int dbWidth READ dbWidth NOTIFY dbWidthChanged)
    Q_PROPERTY(int dbHeight READ dbHeight NOTIFY dbHeightChanged)
    Q_PROPERTY(int dbFps READ dbFps NOTIFY dbFpsChanged)
    Q_PROPERTY(bool valuesChanged READ valuesChanged NOTIFY valuesChangedSignal)
    Q_PROPERTY(CheckListModel* checkListModel READ checkListModel CONSTANT)
    Q_PROPERTY(QPixmap logoPixmap READ logoPixmap NOTIFY logoPixmapChanged)
    Q_PROPERTY(QPixmap framePixmap READ framePixmap NOTIFY framePixmapChanged)

    //导出状态
    Q_PROPERTY(int exportProgress READ exportProgress NOTIFY exportProgressChanged)
    Q_PROPERTY(bool exporting READ isExporting NOTIFY exportingChanged)

public:
    explicit ManagerBackend(QObject *parent = nullptr);
    ~ManagerBackend();

    //配置属性访问器
    QString httpUrl() const;
    void setHttpUrl(const QString &url);
    QString fileUrl() const;
    void setFileUrl(const QString &url);
    QString groupAddress() const;
    void setGroupAddress(const QString &addr);
    QString groupPort() const;
    void setGroupPort(const QString &port);
    int urlTypeIndex() const;
    void setUrlTypeIndex(int index);
    bool addTestEnabled() const;
    void setAddTestEnabled(bool enabled);
    bool mergeChannels() const;
    void setMergeChannels(bool enabled);
    bool addLogoEnabled() const;
    void setAddLogoEnabled(bool enabled);
    bool addHdSuffix() const;
    void setAddHdSuffix(bool enabled);
    QString fccUrl() const;
    void setFccUrl(const QString &url);
    bool fccEnabled() const;
    void setFccEnabled(bool enabled);
    QStringList selectedGroups() const;
    void setSelectedGroups(const QStringList &groups);
    QString importFilePath() const;
    void setImportFilePath(const QString &path);

    //其他属性访问器
    Database::SqlQueryModelQml* browseModel() const;
    QSqlTableModel* channelModel() const;
    int checkIndex() const;
    void setCheckIndex(int index);
    int checkTotal() const;
    bool isChecking() const;
    int checkProgress() const;
    QString checkUrl() const;
    QString checkName() const;
    void setCheckName(const QString &name);
    int checkWidth() const;
    int checkHeight() const;
    int checkFps() const;
    int checkSourceType() const;
    void setCheckSourceType(int type);
    QString checkNotes() const;
    void setCheckNotes(const QString &notes);
    int dbWidth() const;
    int dbHeight() const;
    int dbFps() const;
    bool valuesChanged() const;
    CheckListModel* checkListModel() const;
    QPixmap logoPixmap() const;
    QPixmap framePixmap() const;
    int exportProgress() const;
    bool isExporting() const;

    //文件导入
    Q_INVOKABLE int runImport(const QString &filePath);

    //扫描文件生成
    Q_INVOKABLE bool createScanFile(const QString &filePath);

    //导出
    Q_INVOKABLE void exportM3u(const QString &filePath);
    Q_INVOKABLE void exportTxt(const QString &filePath);
    Q_INVOKABLE void exportExcel(const QString &filePath);

    //流检测
    Q_INVOKABLE void startCheck(bool normalType, bool orderByIp);
    Q_INVOKABLE void nextCheck();
    Q_INVOKABLE void prevCheck();
    Q_INVOKABLE void recheck();
    Q_INVOKABLE bool submitCheck();
    Q_INVOKABLE void removeCheck();

    //频道编辑
    Q_INVOKABLE bool updateChannel(int row, const QString &field, const QString &value);

    //频道检查
    Q_INVOKABLE bool channelExists(const QString &name);
    Q_INVOKABLE QString validateSourceType(int type, int width, int height);

    //表格数据（QML可直接使用）
    Q_INVOKABLE QJsonArray getBrowseData();
    Q_INVOKABLE QJsonArray getChannelData();
    Q_INVOKABLE void sortBrowseData(int column, bool ascending);

    //表格排序
    Q_INVOKABLE void sortBrowseTable(int column);
    Q_INVOKABLE void sortChannelTable(int column, int order);

    //重置
    Q_INVOKABLE void resetConfig();
    Q_INVOKABLE void resetDatabase();

    //刷新
    Q_INVOKABLE void refreshBrowseTable();
    Q_INVOKABLE void refreshChannelTable();

    //外部应用
    Q_INVOKABLE void openScannerApp();

    //初始化
    Q_INVOKABLE void initialize(const QString &configPath, const QString &dbPath);
    void setFrameImageProvider(FrameImageProvider *provider);
    void setLogoImageProvider(LogoImageProvider *provider);
    void setBootstrapPath(const QString &path);

signals:
    void checkIndexChanged();
    void checkTotalChanged();
    void checkingChanged();
    void checkProgressChanged();
    void checkUrlChanged();
    void checkNameChanged();
    void checkWidthChanged();
    void checkHeightChanged();
    void checkFpsChanged();
    void checkSourceTypeChanged();
    void checkNotesChanged();
    void dbWidthChanged();
    void dbHeightChanged();
    void dbFpsChanged();
    void valuesChangedSignal();
    void logoPixmapChanged();
    void framePixmapChanged();
    void exportProgressChanged();
    void exportingChanged();
    void browseDataChanged();
    void channelDataChanged();
    void configChanged();
    void importFilePathChanged();
    void requestReconfigure();

private slots:
    void onStreamProgress(int percent);
    void onStreamSucceeded(const Iptv::Multimedia::StreamInfo &info, const QJsonObject &original);
    void onStreamFailed(const QString &error);
    void onLogoReady(const QPixmap &pixmap);
    void onLogoFailed(const QString &error);
    void onExportProgress(int percent);
    void onExportFinished();

private:
    void buildCheckUrl();

    //服务组件
    Iptv::Core::AppConfig *m_config = nullptr;
    Iptv::Database::DatabaseManager *m_dbManager = nullptr;
    Iptv::Database::ChannelRepository *m_channelRepo = nullptr;
    Iptv::Database::SourceRepository *m_sourceRepo = nullptr;
    Iptv::Network::HttpClient *m_httpClient = nullptr;
    Iptv::Network::LogoFetcher *m_logoFetcher = nullptr;
    Iptv::Multimedia::StreamProbe *m_streamProbe = nullptr;
    Iptv::Export::XlsxExporter *m_xlsxExporter = nullptr;
    Iptv::Logic::ChannelService *m_channelService = nullptr;
    Iptv::Logic::CheckService *m_checkService = nullptr;
    Iptv::Logic::GuideManager *m_guideManager = nullptr;

    //模型
    Database::SqlQueryModelQml *m_queryModel = nullptr;
    QSqlTableModel *m_tableModel = nullptr;
    CheckListModel *m_checkListModel = nullptr;

    //检测状态
    QJsonArray m_checkList;
    int m_checkIndex = -1;
    bool m_checkDataChange = false;
    bool m_suppressComboSignal = false;
    QString m_currentCheckUrl;

    //检测输入
    QString m_checkName;
    int m_checkSourceType = 0;
    QString m_checkNotes;

    //检测结果
    int m_checkWidth = 0;
    int m_checkHeight = 0;
    int m_checkFps = 0;
    int m_dbWidth = 0;
    int m_dbHeight = 0;
    int m_dbFps = 0;
    int m_checkProgress = 0;
    QPixmap m_logoPixmap;
    QPixmap m_framePixmap;
    FrameImageProvider *m_frameImageProvider = nullptr;
    LogoImageProvider *m_logoImageProvider = nullptr;

    //导出状态
    int m_exportProgress = 0;
    bool m_exporting = false;
    QString m_bootstrapPath;
    QString m_importFilePath;
};

} // namespace Iptv::Ui

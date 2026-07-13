#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

namespace Iptv::Ui {
class ScanResultModel;
}

namespace Iptv::Logic {
class ScanService;
}

namespace Iptv::Ui {

/**
 * @brief 扫描器QML桥接类
 *        将扫描业务逻辑暴露给QML界面
 */
class ScannerBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool scanning READ isScanning NOTIFY scanningChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString progressText READ progressText NOTIFY progressChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QString statusText2 READ statusText2 NOTIFY statusText2Changed)
    Q_PROPERTY(int resultCount READ resultCount NOTIFY resultCountChanged)
    Q_PROPERTY(ScanResultModel* resultModel READ resultModel CONSTANT)

    //配置属性
    Q_PROPERTY(QString scanUrl READ scanUrl WRITE setScanUrl NOTIFY configChanged)
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY configChanged)
    Q_PROPERTY(int threadMax READ threadMax WRITE setThreadMax NOTIFY configChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY configChanged)
    Q_PROPERTY(bool autoStep READ autoStep WRITE setAutoStep NOTIFY configChanged)
    Q_PROPERTY(bool addTs READ addTs WRITE setAddTs NOTIFY configChanged)
    Q_PROPERTY(bool slowScan READ slowScan WRITE setSlowScan NOTIFY configChanged)
    Q_PROPERTY(bool useUrl READ useUrl WRITE setUseUrl NOTIFY configChanged)

public:
    explicit ScannerBackend(QObject *parent = nullptr);

    //属性访问器
    bool isScanning() const;
    int progress() const;
    QString progressText() const;
    QString statusText() const;
    QString statusText2() const;
    int resultCount() const;
    ScanResultModel* resultModel() const;

    //配置属性访问器
    QString scanUrl() const;
    void setScanUrl(const QString &url);
    QString filePath() const;
    void setFilePath(const QString &path);
    int threadMax() const;
    void setThreadMax(int max);
    int timeout() const;
    void setTimeout(int ms);
    bool autoStep() const;
    void setAutoStep(bool enabled);
    bool addTs() const;
    void setAddTs(bool enabled);
    bool slowScan() const;
    void setSlowScan(bool enabled);
    bool useUrl() const;
    void setUseUrl(bool enabled);

    //Q_INVOKABLE 方法供QML调用
    Q_INVOKABLE void startScan(const QString &urlInput, bool useUrl);
    Q_INVOKABLE void stopScan();
    Q_INVOKABLE void clearResults();
    Q_INVOKABLE bool saveResults(const QString &path);
    Q_INVOKABLE void loadFile(const QString &path);
    Q_INVOKABLE void loadConfig();
    Q_INVOKABLE void saveConfig();
    Q_INVOKABLE void restoreDefaults();

signals:
    void scanningChanged();
    void progressChanged();
    void statusTextChanged();
    void statusText2Changed();
    void resultCountChanged();
    void configChanged();
    void logLineAdded(const QString &line);
    void logCleared();

private slots:
    void onScanResult(const QString &url, int width, int height, int fps);
    void onScanProgress(int pos, int count, int okCount, int errorCount);
    void onCurrentUrlChanged(const QString &url);
    void onScanFinished();

private:
    Logic::ScanService *m_scanService;
    ScanResultModel *m_resultModel;
    QStringList m_resultList;

    //状态
    int m_pos = 0;
    int m_total = 0;
    int m_okCount = 0;
    int m_errorCount = 0;
    bool m_scanning = false;
    QString m_currentUrl;

    //配置
    QString m_scanUrl;
    QString m_filePath;
    int m_threadMax = 8;
    int m_timeout = 500;
    bool m_autoStep = false;
    bool m_addTs = false;
    bool m_slowScan = false;
    bool m_useUrl = true;
};

} // namespace Iptv::Ui

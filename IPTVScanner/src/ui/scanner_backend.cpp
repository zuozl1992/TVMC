#include "scanner_backend.h"
#include "models/scan_result_model.h"
#include "logic/scan_service.h"
#include "logic/url_builder.h"
#include "core/config/appconfig.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

namespace Iptv::Ui {

ScannerBackend::ScannerBackend(QObject *parent)
    : QObject(parent)
    , m_scanService(new Logic::ScanService(this))
    , m_resultModel(new ScanResultModel(this))
{
    //连接扫描服务信号
    connect(m_scanService, &Logic::ScanService::scanResult,
            this, [this](const Multimedia::StreamInfo &info) {
                m_resultModel->addResult(info);
                onScanResult(info.url, info.width, info.height, info.fps);
            });
    connect(m_scanService, &Logic::ScanService::scanProgress,
            this, &ScannerBackend::onScanProgress);
    connect(m_scanService, &Logic::ScanService::currentUrlChanged,
            this, &ScannerBackend::onCurrentUrlChanged);
    connect(m_scanService, &Logic::ScanService::scanFinished,
            this, &ScannerBackend::onScanFinished);
}

//属性访问器实现
bool ScannerBackend::isScanning() const { return m_scanning; }
int ScannerBackend::progress() const { return m_total > 0 ? static_cast<int>(static_cast<double>(m_pos) * 100.0 / m_total) : 0; }
QString ScannerBackend::progressText() const { return QString("%1/%2").arg(m_pos).arg(m_total); }
QString ScannerBackend::statusText() const { return m_currentUrl; }
QString ScannerBackend::statusText2() const { return tr("成功：%1，失败：%2").arg(m_okCount).arg(m_errorCount); }
int ScannerBackend::resultCount() const { return m_resultList.size(); }
ScanResultModel* ScannerBackend::resultModel() const { return m_resultModel; }

//配置属性访问器
QString ScannerBackend::scanUrl() const { return m_scanUrl; }
void ScannerBackend::setScanUrl(const QString &url) { if (m_scanUrl != url) { m_scanUrl = url; emit configChanged(); saveConfig(); } }
QString ScannerBackend::filePath() const { return m_filePath; }
void ScannerBackend::setFilePath(const QString &path) { if (m_filePath != path) { m_filePath = path; emit configChanged(); saveConfig(); } }
int ScannerBackend::threadMax() const { return m_threadMax; }
void ScannerBackend::setThreadMax(int max) { if (m_threadMax != max) { m_threadMax = max; emit configChanged(); saveConfig(); } }
int ScannerBackend::timeout() const { return m_timeout; }
void ScannerBackend::setTimeout(int ms) { if (m_timeout != ms) { m_timeout = ms; emit configChanged(); saveConfig(); } }
bool ScannerBackend::autoStep() const { return m_autoStep; }
void ScannerBackend::setAutoStep(bool enabled) { if (m_autoStep != enabled) { m_autoStep = enabled; emit configChanged(); saveConfig(); } }
bool ScannerBackend::addTs() const { return m_addTs; }
void ScannerBackend::setAddTs(bool enabled) { if (m_addTs != enabled) { m_addTs = enabled; emit configChanged(); saveConfig(); } }
bool ScannerBackend::slowScan() const { return m_slowScan; }
void ScannerBackend::setSlowScan(bool enabled) { if (m_slowScan != enabled) { m_slowScan = enabled; emit configChanged(); saveConfig(); } }
bool ScannerBackend::useUrl() const { return m_useUrl; }
void ScannerBackend::setUseUrl(bool enabled) { if (m_useUrl != enabled) { m_useUrl = enabled; emit configChanged(); saveConfig(); } }

void ScannerBackend::startScan(const QString &urlInput, bool useUrl)
{
    //展开URL模板
    QStringList urls;
    if (useUrl)
        urls = Logic::UrlBuilder::expand(urlInput);
    else
        urls = Logic::UrlBuilder::expandFromFile(urlInput);

    if (urls.isEmpty()) {
        return;
    }

    //清空上次结果
    clearResults();

    //配置扫描服务
    m_scanService->setUrlList(urls);
    m_scanService->setTimeout(m_timeout);
    m_scanService->setThreadMax(m_threadMax);
    m_scanService->setAutoStep(m_autoStep);
    m_scanService->setSlowScan(m_slowScan);

    //重置状态
    m_pos = 0;
    m_total = urls.length();
    m_okCount = 0;
    m_errorCount = 0;
    m_scanning = true;
    m_currentUrl.clear();

    emit progressChanged();
    emit statusTextChanged();
    emit statusText2Changed();
    emit scanningChanged();

    m_scanService->start();
}

void ScannerBackend::stopScan()
{
    m_scanning = false;
    m_currentUrl = tr("扫描已停止");
    m_scanService->stop();
    emit statusTextChanged();
    emit scanningChanged();
}

void ScannerBackend::clearResults()
{
    m_resultModel->clear();
    m_resultList.clear();
    emit resultCountChanged();
    emit logCleared();
}

bool ScannerBackend::saveResults(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    QTextStream stream(&file);
    for (const QString &line : m_resultList) {
        stream << line << "\n";
    }
    file.close();
    return true;
}

void ScannerBackend::loadFile(const QString &path)
{
    //将文件路径设置到文件路径配置
    setFilePath(path);
}

void ScannerBackend::loadConfig()
{
    Core::AppConfig *config = Core::AppConfig::instance();
    m_scanUrl = config->scanUrl();
    m_filePath = config->modelFilePath();
    m_threadMax = config->threadMax();
    m_timeout = config->timeout();
    m_autoStep = config->autoStep();
    m_addTs = config->addTs();
    m_slowScan = config->slowScan();
    m_useUrl = config->useUrl();
    emit configChanged();

    //首次运行时确保默认值写入磁盘
    saveConfig();
}

void ScannerBackend::saveConfig()
{
    Core::AppConfig *config = Core::AppConfig::instance();
    config->setScanUrl(m_scanUrl);
    config->setModelFilePath(m_filePath);
    config->setThreadMax(m_threadMax);
    config->setTimeout(m_timeout);
    config->setAutoStep(m_autoStep);
    config->setAddTs(m_addTs);
    config->setSlowScan(m_slowScan);
    config->setUseUrl(m_useUrl);
}

void ScannerBackend::restoreDefaults()
{
    m_scanUrl = "http://192.168.1.1:12345/udp/239.49.0.{[1-255]}:{6000#[8000-9999]}";
    m_filePath.clear();
    m_threadMax = 8;
    m_timeout = 500;
    m_autoStep = false;
    m_addTs = false;
    m_slowScan = false;
    m_useUrl = true;
    emit configChanged();
    saveConfig();
}

void ScannerBackend::onScanResult(const QString &url, int width, int height, int fps)
{
    //格式化扫描结果日志
    QString log;
    if (m_addTs)
        log = QString("待整理 TS %1x%2(%3),%4").arg(width).arg(height).arg(fps).arg(url);
    else
        log = QString("%1x%2(%3),%4").arg(width).arg(height).arg(fps).arg(url);

    m_resultList.append(log);
    emit logLineAdded(log);
    emit resultCountChanged();
}

void ScannerBackend::onScanProgress(int pos, int count, int okCount, int errorCount)
{
    m_pos = pos;
    m_total = count;
    m_okCount = okCount;
    m_errorCount = errorCount;

    emit progressChanged();
    emit statusText2Changed();
}

void ScannerBackend::onCurrentUrlChanged(const QString &url)
{
    m_currentUrl = url;
    emit statusTextChanged();
}

void ScannerBackend::onScanFinished()
{
    m_scanning = false;
    m_pos = m_total;
    m_currentUrl = tr("扫描完成");
    emit statusTextChanged();
    emit progressChanged();
    emit scanningChanged();
}

} // namespace Iptv::Ui

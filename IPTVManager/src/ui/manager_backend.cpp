#include "manager_backend.h"
#include "models/check_list_model.h"

#include "core/config/appconfig.h"
#include "core/config/bootstrap.h"
#include "database/database_manager.h"
#include "database/channel_repository.h"
#include "database/source_repository.h"
#include "database/schema_manager.h"
#include "network/httpclient.h"
#include "network/logo_fetcher.h"
#include "multimedia/stream_probe.h"
#include "multimedia/ffmpeg_utils.h"
#include "export/m3u_exporter.h"
#include "export/txt_exporter.h"
#include "export/csv_exporter.h"
#include "export/xlsx_exporter.h"
#include "export/scanfile_generator.h"
#include "logic/channel_service.h"
#include "logic/check_service.h"
#include "logic/url_builder.h"
#include "logic/guide_manager.h"

#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QProcess>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QSettings>

namespace Iptv::Ui {

ManagerBackend::ManagerBackend(QObject *parent)
    : QObject(parent)
    , m_checkListModel(new CheckListModel(this))
{
}

ManagerBackend::~ManagerBackend()
{
}

void ManagerBackend::initialize(const QString &configPath, const QString &dbPath)
{
    //初始化配置
    m_config = Core::AppConfig::instance();
    m_config->init(configPath);

    //连接配置变更信号
    connect(m_config, &Core::AppConfig::configChanged,
            this, &ManagerBackend::configChanged);

    //初始化数据库
    m_dbManager = new Database::DatabaseManager(dbPath, this);
    if (!m_dbManager->open()) {
        qCritical() << "Failed to open database";
        return;
    }

    //确保数据库表结构存在
    Database::SchemaManager::ensureSchema(*m_dbManager->database());

    m_channelRepo = new Database::ChannelRepository(m_dbManager->database());
    m_sourceRepo = new Database::SourceRepository(m_dbManager->database());

    //初始化网络
    m_httpClient = new Network::HttpClient(this);
    m_logoFetcher = new Network::LogoFetcher(m_httpClient, this);

    //初始化多媒体
    m_streamProbe = new Multimedia::StreamProbe(this);

    //初始化导出
    m_xlsxExporter = new Export::XlsxExporter(this);

    //初始化指南管理器
    m_guideManager = new Logic::GuideManager(m_httpClient, this);
    m_guideManager->init(QFileInfo(configPath).absolutePath());

    //初始化业务服务
    m_channelService = new Logic::ChannelService(m_channelRepo, m_sourceRepo, m_guideManager, this);
    m_checkService = new Logic::CheckService(m_streamProbe, m_sourceRepo, m_channelRepo, this);

    //初始化模型（使用QML兼容的模型）
    m_queryModel = new Database::SqlQueryModelQml(this);
    m_queryModel->setQuerySql(
        "SELECT ti.id, ti.channel_id, ti.name, ti.\"group\", tsi.ip, tsi.port, "
        "tsi.width, tsi.height, tsi.fps, st.name as type, tsi.notes "
        "FROM tv_info ti "
        "JOIN tv_source_info tsi ON ti.id = tsi.tv_id "
        "JOIN source_type st ON st.id = tsi.type "
        "ORDER BY ti.channel_id, tsi.width DESC, tsi.fps DESC",
        *m_dbManager->database());
    m_tableModel = new QSqlTableModel(this, *m_dbManager->database());
    m_tableModel->setTable("tv_info");
    m_tableModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_tableModel->setSort(1, Qt::AscendingOrder);
    m_tableModel->select();

    //连接信号
    connect(m_streamProbe, &Multimedia::StreamProbe::progressChanged,
            this, &ManagerBackend::onStreamProgress);
    connect(m_streamProbe, &Multimedia::StreamProbe::probeSucceeded,
            this, &ManagerBackend::onStreamSucceeded);
    connect(m_streamProbe, &Multimedia::StreamProbe::probeFailed,
            this, &ManagerBackend::onStreamFailed);

    connect(m_logoFetcher, &Network::LogoFetcher::imageReady,
            this, &ManagerBackend::onLogoReady);
    connect(m_logoFetcher, &Network::LogoFetcher::fetchFailed,
            this, &ManagerBackend::onLogoFailed);

    connect(m_xlsxExporter, &Export::XlsxExporter::progressChanged,
            this, &ManagerBackend::onExportProgress);

    //触发配置变更信号，强制QML重新绑定
    emit configChanged();
}

void ManagerBackend::setFrameImageProvider(FrameImageProvider *provider)
{
    m_frameImageProvider = provider;
}

void ManagerBackend::setLogoImageProvider(LogoImageProvider *provider)
{
    m_logoImageProvider = provider;
}

void ManagerBackend::setBootstrapPath(const QString &path)
{
    m_bootstrapPath = path;
}

//配置属性访问器实现
QString ManagerBackend::httpUrl() const { return m_config ? m_config->httpUrl() : QString(); }
void ManagerBackend::setHttpUrl(const QString &url) { if (m_config) m_config->setHttpUrl(url); }
QString ManagerBackend::fileUrl() const { return m_config ? m_config->fileUrl() : QString(); }
void ManagerBackend::setFileUrl(const QString &url) { if (m_config) m_config->setFileUrl(url); }
QString ManagerBackend::groupAddress() const { return m_config ? m_config->groupAddress() : QString(); }
void ManagerBackend::setGroupAddress(const QString &addr) { if (m_config) m_config->setGroupAddress(addr); }
QString ManagerBackend::groupPort() const { return m_config ? m_config->groupPort() : QString(); }
void ManagerBackend::setGroupPort(const QString &port) { if (m_config) m_config->setGroupPort(port); }
int ManagerBackend::urlTypeIndex() const { return m_config ? m_config->urlTypeIndex() : 0; }
void ManagerBackend::setUrlTypeIndex(int index) { if (m_config) m_config->setUrlTypeIndex(index); }
bool ManagerBackend::addTestEnabled() const { return m_config ? m_config->addTestEnabled() : false; }
void ManagerBackend::setAddTestEnabled(bool enabled) { if (m_config) m_config->setAddTestEnabled(enabled); }
bool ManagerBackend::mergeChannels() const { return m_config ? m_config->mergeChannels() : false; }
void ManagerBackend::setMergeChannels(bool enabled) { if (m_config) m_config->setMergeChannels(enabled); }
bool ManagerBackend::addLogoEnabled() const { return m_config ? m_config->addLogoEnabled() : false; }
void ManagerBackend::setAddLogoEnabled(bool enabled) { if (m_config) m_config->setAddLogoEnabled(enabled); }
bool ManagerBackend::addHdSuffix() const { return m_config ? m_config->addHdSuffix() : false; }
void ManagerBackend::setAddHdSuffix(bool enabled) { if (m_config) m_config->setAddHdSuffix(enabled); }
QString ManagerBackend::fccUrl() const { return m_config ? m_config->fccUrl() : QString(); }
void ManagerBackend::setFccUrl(const QString &url) { if (m_config) m_config->setFccUrl(url); }
bool ManagerBackend::fccEnabled() const { return m_config ? m_config->fccEnabled() : false; }
void ManagerBackend::setFccEnabled(bool enabled) { if (m_config) m_config->setFccEnabled(enabled); }
QStringList ManagerBackend::selectedGroups() const { return m_config ? m_config->selectedGroups() : QStringList(); }
void ManagerBackend::setSelectedGroups(const QStringList &groups) { if (m_config) m_config->setSelectedGroups(groups); }
QString ManagerBackend::importFilePath() const { return m_importFilePath; }
void ManagerBackend::setImportFilePath(const QString &path) { m_importFilePath = path; emit importFilePathChanged(); }

//其他属性访问器实现
Database::SqlQueryModelQml* ManagerBackend::browseModel() const { return m_queryModel; }
QSqlTableModel* ManagerBackend::channelModel() const { return m_tableModel; }
int ManagerBackend::checkIndex() const { return m_checkIndex; }
void ManagerBackend::setCheckIndex(int index) { m_checkIndex = index; emit checkIndexChanged(); }
int ManagerBackend::checkTotal() const { return m_checkList.size(); }
bool ManagerBackend::isChecking() const { return m_streamProbe && m_streamProbe->isRunning(); }
int ManagerBackend::checkProgress() const { return m_checkProgress; }
QString ManagerBackend::checkUrl() const { return m_currentCheckUrl; }
QString ManagerBackend::checkName() const { return m_checkName; }
void ManagerBackend::setCheckName(const QString &name) { m_checkName = name; emit checkNameChanged(); }
int ManagerBackend::checkWidth() const { return m_checkWidth; }
int ManagerBackend::checkHeight() const { return m_checkHeight; }
int ManagerBackend::checkFps() const { return m_checkFps; }
int ManagerBackend::checkSourceType() const { return m_checkSourceType; }
void ManagerBackend::setCheckSourceType(int type) { m_checkSourceType = type; emit checkSourceTypeChanged(); }
QString ManagerBackend::checkNotes() const { return m_checkNotes; }
void ManagerBackend::setCheckNotes(const QString &notes) { m_checkNotes = notes; emit checkNotesChanged(); }
int ManagerBackend::dbWidth() const { return m_dbWidth; }
int ManagerBackend::dbHeight() const { return m_dbHeight; }
int ManagerBackend::dbFps() const { return m_dbFps; }
bool ManagerBackend::valuesChanged() const {
    return (m_checkWidth != m_dbWidth) || (m_checkHeight != m_dbHeight) || (m_checkFps != m_dbFps);
}
CheckListModel* ManagerBackend::checkListModel() const { return m_checkListModel; }
QPixmap ManagerBackend::logoPixmap() const { return m_logoPixmap; }
QPixmap ManagerBackend::framePixmap() const { return m_framePixmap; }
int ManagerBackend::exportProgress() const { return m_exportProgress; }
bool ManagerBackend::isExporting() const { return m_exporting; }

QJsonArray ManagerBackend::getBrowseData()
{
    QJsonArray result;
    if (!m_queryModel) return result;

    int totalRows = m_queryModel->rowCount();

    for (int row = 0; row < totalRows; ++row) {
        QJsonObject obj;
        obj["channelId"] = m_queryModel->data(m_queryModel->index(row, 1)).toString();
        obj["name"] = m_queryModel->data(m_queryModel->index(row, 2)).toString();
        obj["group"] = m_queryModel->data(m_queryModel->index(row, 3)).toString();
        obj["ip"] = m_queryModel->data(m_queryModel->index(row, 4)).toString();
        obj["port"] = m_queryModel->data(m_queryModel->index(row, 5)).toString();
        obj["width"] = m_queryModel->data(m_queryModel->index(row, 6)).toString();
        obj["height"] = m_queryModel->data(m_queryModel->index(row, 7)).toString();
        obj["fps"] = m_queryModel->data(m_queryModel->index(row, 8)).toString();
        obj["type"] = m_queryModel->data(m_queryModel->index(row, 9)).toString();
        result.append(obj);
    }
    return result;
}

void ManagerBackend::sortBrowseData(int column, bool ascending)
{
    //列映射：0=channelId, 1=name, 2=group, 3=ip, 4=port, 5=width, 6=height, 7=fps, 8=type
    QString sql = "SELECT ti.id, ti.channel_id, ti.name, ti.\"group\", tsi.ip, tsi.port, "
                  "tsi.width, tsi.height, tsi.fps, st.name as type, tsi.notes "
                  "FROM tv_info ti "
                  "JOIN tv_source_info tsi ON ti.id = tsi.tv_id "
                  "JOIN source_type st ON st.id = tsi.type ";
    QString order;
    QString dir = ascending ? "ASC" : "DESC";

    switch (column) {
    case 0: order = "ORDER BY ti.channel_id " + dir + ", tsi.width DESC, tsi.fps DESC"; break;
    case 1: order = "ORDER BY ti.name " + dir + ", tsi.width DESC, tsi.fps DESC"; break;
    case 2: order = "ORDER BY ti.\"group\" " + dir + ", ti.channel_id ASC"; break;
    case 3: order = "ORDER BY json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[0]') " + dir + ","
                   "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[1]'),"
                   "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[2]'),"
                   "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[3]')"; break;
    case 4: order = "ORDER BY tsi.port " + dir + ", tsi.width DESC, tsi.fps DESC"; break;
    case 5: order = "ORDER BY tsi.width " + dir + ", ti.channel_id ASC, tsi.fps DESC"; break;
    case 6: order = "ORDER BY tsi.height " + dir + ", ti.channel_id ASC, tsi.fps DESC"; break;
    case 7: order = "ORDER BY tsi.fps " + dir + ", ti.channel_id ASC, tsi.width DESC"; break;
    case 8: order = "ORDER BY tsi.type " + dir + ", ti.channel_id ASC, tsi.fps DESC"; break;
    default: order = "ORDER BY ti.channel_id " + dir + ", tsi.width DESC, tsi.fps DESC"; break;
    }

    m_queryModel->setQuerySql(sql + order, *m_dbManager->database());
    emit browseDataChanged();
}

QJsonArray ManagerBackend::getChannelData()
{
    QJsonArray result;
    if (!m_tableModel) return result;

    for (int row = 0; row < m_tableModel->rowCount(); ++row) {
        QJsonObject obj;
        obj["channelId"] = m_tableModel->data(m_tableModel->index(row, 1)).toString();
        obj["name"] = m_tableModel->data(m_tableModel->index(row, 2)).toString();
        obj["group"] = m_tableModel->data(m_tableModel->index(row, 3)).toString();
        obj["city"] = m_tableModel->data(m_tableModel->index(row, 4)).toString();
        obj["describe"] = m_tableModel->data(m_tableModel->index(row, 5)).toString();
        obj["notes"] = m_tableModel->data(m_tableModel->index(row, 6)).toString();
        obj["logoName"] = m_tableModel->data(m_tableModel->index(row, 7)).toString();
        result.append(obj);
    }
    return result;
}

int ManagerBackend::runImport(const QString &filePath)
{
    int count = m_channelService->importFromFile(filePath);
    if (count > 0) {
        refreshBrowseTable();
        refreshChannelTable();
        emit browseDataChanged();
        emit channelDataChanged();
    }
    return count;
}

bool ManagerBackend::createScanFile(const QString &filePath)
{
    Export::ExportOptions opts;
    opts.urlPrefix = m_config->httpUrl();
    //根据协议类型索引确定协议（0=udp, 1=rtp）
    opts.urlType = m_config->urlTypeIndex() == 1 ? "rtp" : "udp";
    opts.fccUrl = m_config->fccUrl();
    opts.fccEnabled = m_config->fccEnabled();

    //获取所有已存在的IP
    QStringList existingIps = m_sourceRepo->allIps();
    
    //如果勾选"包含测试"，则排除TS类型的IP（让它们被扫描）
    if (m_config->addTestEnabled()) {
        QStringList tsIps = m_sourceRepo->tsIps();
        for (const QString &ip : tsIps) {
            existingIps.removeAll(ip);
        }
    }

    QStringList scanIps = Logic::UrlBuilder::buildScanIps(
        m_config->groupAddress(), m_config->groupPort(), existingIps);

    Export::ScanFileGenerator generator;
    //不再单独导出测试URL，而是通过过滤TS IP来处理
    return generator.generate(filePath, {}, scanIps, opts);
}

void ManagerBackend::exportM3u(const QString &filePath)
{
    Export::ExportOptions opts;
    opts.urlPrefix = m_config->httpUrl();
    opts.urlType = m_config->urlTypeIndex() == 1 ? "rtp" : "udp";
    opts.fccUrl = m_config->fccUrl();
    opts.fccEnabled = m_config->fccEnabled();
    opts.addLogo = m_config->addLogoEnabled();
    opts.addHdSuffix = m_config->addHdSuffix();
    opts.mergeChannels = m_config->mergeChannels();
    opts.logoBaseUrl = m_config->fileUrl();
    opts.groups = m_config->selectedGroups();

    QJsonArray channelList;
    for (const QString &group : opts.groups) {
        QJsonArray list = m_sourceRepo->getTvListByGroup(group, true);
        for (const QJsonValue &val : list) {
            channelList.append(val);
        }
    }

    Export::M3uExporter exporter;
    exporter.exportToFile(filePath, channelList, opts);
}

void ManagerBackend::exportTxt(const QString &filePath)
{
    Export::ExportOptions opts;
    opts.urlPrefix = m_config->httpUrl();
    opts.urlType = m_config->urlTypeIndex() == 1 ? "rtp" : "udp";
    opts.fccUrl = m_config->fccUrl();
    opts.fccEnabled = m_config->fccEnabled();
    opts.mergeChannels = m_config->mergeChannels();
    opts.groups = m_config->selectedGroups();

    QHash<QString, QJsonArray> groupedLists;
    for (const QString &group : opts.groups) {
        QJsonArray list = m_sourceRepo->getTvListByGroup(group, false);
        if (!list.isEmpty()) {
            groupedLists[group] = list;
        }
    }

    Export::TxtExporter exporter;
    exporter.exportToFile(filePath, groupedLists, opts, opts.groups);
}

void ManagerBackend::exportExcel(const QString &filePath)
{
    m_exporting = true;
    emit exportingChanged();

    if (filePath.endsWith(".xlsx")) {
        m_xlsxExporter->exportToFile(filePath, m_queryModel, m_tableModel);
    } else {
        Export::CsvExporter exporter;
        exporter.exportToFile(filePath, m_queryModel);
    }

    m_exporting = false;
    emit exportingChanged();
}

void ManagerBackend::startCheck(bool normalType, bool orderByIp)
{
    m_checkList = m_checkService->loadCheckList(normalType, orderByIp ? 0 : 1);
    m_checkIndex = 0;
    m_checkProgress = 0;

    //更新检测列表模型
    m_checkListModel->setList(m_checkList);

    emit checkIndexChanged();
    emit checkTotalChanged();
    emit checkProgressChanged();

    if (m_checkList.isEmpty()) {
        return;
    }

    //开始第一个检测
    QJsonObject source = m_checkList[0].toObject();
    m_dbWidth = source.value("width").toInt();
    m_dbHeight = source.value("height").toInt();
    m_dbFps = source.value("fps").toInt();
    buildCheckUrl();
    m_checkService->startCheck(source, m_currentCheckUrl);
    emit checkUrlChanged();
    emit checkingChanged();
}

void ManagerBackend::nextCheck()
{
    if (m_checkIndex >= m_checkList.size() - 1) return;

    m_checkIndex++;
    emit checkIndexChanged();

    QJsonObject source = m_checkList[m_checkIndex].toObject();
    m_dbWidth = source.value("width").toInt();
    m_dbHeight = source.value("height").toInt();
    m_dbFps = source.value("fps").toInt();
    buildCheckUrl();
    m_checkService->startCheck(source, m_currentCheckUrl);
    emit checkUrlChanged();
    emit valuesChangedSignal();
}

void ManagerBackend::prevCheck()
{
    if (m_checkIndex <= 0) return;

    m_checkIndex--;
    emit checkIndexChanged();

    QJsonObject source = m_checkList[m_checkIndex].toObject();
    m_dbWidth = source.value("width").toInt();
    m_dbHeight = source.value("height").toInt();
    m_dbFps = source.value("fps").toInt();
    buildCheckUrl();
    m_checkService->startCheck(source, m_currentCheckUrl);
    emit checkUrlChanged();
}

void ManagerBackend::recheck()
{
    if (m_checkIndex < 0 || m_checkIndex >= m_checkList.size()) return;

    QJsonObject source = m_checkList[m_checkIndex].toObject();
    m_dbWidth = source.value("width").toInt();
    m_dbHeight = source.value("height").toInt();
    m_dbFps = source.value("fps").toInt();
    buildCheckUrl();
    m_checkService->startCheck(source, m_currentCheckUrl);
    emit checkUrlChanged();
    emit checkingChanged();
}

bool ManagerBackend::submitCheck()
{
    if (m_checkIndex < 0 || m_checkIndex >= m_checkList.size()) {
        qWarning() << "submitCheck: invalid index" << m_checkIndex;
        return false;
    }

    QJsonObject source = m_checkList[m_checkIndex].toObject();

    int tvId = m_channelService->getOrCreateChannel(m_checkName);
    if (tvId < 0) {
        qWarning() << "submitCheck: failed to get/create channel" << m_checkName;
        return false;
    }

    Database::TvSource tvSource;
    tvSource.tvId = tvId;
    tvSource.ip = source.value("ip").toString();
    tvSource.port = source.value("port").toInt();
    tvSource.width = m_checkWidth;
    tvSource.height = m_checkHeight;
    tvSource.fps = m_checkFps;
    tvSource.type = m_checkSourceType;
    tvSource.notes = m_checkNotes;

    if (m_channelService->addSource(tvId, tvSource)) {
        m_checkDataChange = false;
        //更新数据库值为当前检测值
        m_dbWidth = m_checkWidth;
        m_dbHeight = m_checkHeight;
        m_dbFps = m_checkFps;
        //同步更新m_checkList中的数据
        QJsonObject updatedSource = m_checkList[m_checkIndex].toObject();
        updatedSource["width"] = m_checkWidth;
        updatedSource["height"] = m_checkHeight;
        updatedSource["fps"] = m_checkFps;
        m_checkList[m_checkIndex] = updatedSource;
        refreshBrowseTable();
        refreshChannelTable();
        emit browseDataChanged();
        emit channelDataChanged();
        emit valuesChangedSignal();
        return true;
    }
    qWarning() << "submitCheck: addSource failed";
    return false;
}

void ManagerBackend::removeCheck()
{
    if (m_checkIndex < 0 || m_checkIndex >= m_checkList.size()) return;

    QJsonObject source = m_checkList[m_checkIndex].toObject();
    QString ip = source.value("ip").toString();

    m_channelService->removeSource(ip);
    m_checkList.removeAt(m_checkIndex);

    if (m_checkIndex >= m_checkList.size()) {
        m_checkIndex = m_checkList.size() - 1;
    }

    m_checkListModel->setList(m_checkList);
    emit checkIndexChanged();
    emit checkTotalChanged();
    refreshBrowseTable();
}

bool ManagerBackend::updateChannel(int row, const QString &field, const QString &value)
{
    if (!m_tableModel || row < 0 || row >= m_tableModel->rowCount()) {
        qWarning() << "updateChannel: invalid row" << row;
        return false;
    }

    int column = -1;
    if (field == "channelId") column = 1;
    else if (field == "name") column = 2;
    else if (field == "group") column = 3;
    else if (field == "city") column = 4;
    else if (field == "describe") column = 5;
    else if (field == "notes") column = 6;
    else if (field == "logoName") column = 7;
    else {
        qWarning() << "updateChannel: unknown field" << field;
        return false;
    }

    QModelIndex idx = m_tableModel->index(row, column);
    bool result = m_tableModel->setData(idx, value);
    qInfo() << "updateChannel: row=" << row << "field=" << field << "value=" << value << "result=" << result;
    return result;
}

bool ManagerBackend::channelExists(const QString &name)
{
    if (!m_channelRepo) return false;
    return m_channelRepo->findIdByName(name) >= 0;
}

QString ManagerBackend::validateSourceType(int type, int width, int height)
{
    //type: 0=TS, 1=SD, 2=HD, 3=4K, 4=8K
    //TS类型不做验证
    if (type == 0) return QString();

    //SD: 720p及以下 (1280x720, 720x480, 720x576等)
    if (type == 1) {
        if ((width == 1280 && height == 720) ||
            (width == 720 && (height == 480 || height == 576)) ||
            (width == 640 && height == 480) ||
            (width == 480 && height == 576)) {
            return QString();
        }
        return tr("SD类型应为720p及以下分辨率（1280x720/720x480/720x576）");
    }

    //HD: 1080p (1920x1080)
    if (type == 2) {
        if (width == 1920 && height == 1080) {
            return QString();
        }
        return tr("HD类型应为1080p分辨率（1920x1080）");
    }

    //4K: 2160p (3840x2160)
    if (type == 3) {
        if (width == 3840 && height == 2160) {
            return QString();
        }
        return tr("4K类型应为2160p分辨率（3840x2160）");
    }

    //8K: 4320p (7680x4320)
    if (type == 4) {
        if (width == 7680 && height == 4320) {
            return QString();
        }
        return tr("8K类型应为4320p分辨率（7680x4320）");
    }

    return tr("未知的类型");
}

void ManagerBackend::sortBrowseTable(int column)
{
    QString sql = "SELECT ti.id, ti.channel_id, ti.name, ti.\"group\", tsi.ip, tsi.port, "
                  "tsi.width, tsi.height, tsi.fps, st.name as type, tsi.notes "
                  "FROM tv_info ti "
                  "JOIN tv_source_info tsi ON ti.id = tsi.tv_id "
                  "JOIN source_type st ON st.id = tsi.type ";

    switch (column) {
    case 1: sql += "ORDER BY ti.channel_id, tsi.width DESC, tsi.fps DESC"; break;
    case 2: sql += "ORDER BY ti.name, tsi.width DESC, tsi.fps DESC"; break;
    case 3: sql += "ORDER BY ti.\"group\", ti.channel_id, tsi.width DESC, tsi.fps DESC"; break;
    case 4: sql += "ORDER BY json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[0]'),"
                   "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[1]'),"
                   "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[2]'),"
                   "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[3]'),"
                   "tsi.width DESC, tsi.fps DESC"; break;
    case 5: sql += "ORDER BY tsi.port, tsi.width DESC, tsi.fps DESC"; break;
    case 6: sql += "ORDER BY tsi.width DESC, ti.channel_id, tsi.fps DESC"; break;
    case 7: sql += "ORDER BY tsi.height DESC, ti.channel_id, tsi.fps DESC"; break;
    case 8: sql += "ORDER BY tsi.fps DESC, ti.channel_id, tsi.width DESC"; break;
    case 9: sql += "ORDER BY tsi.type DESC, ti.channel_id, tsi.fps DESC"; break;
    default: sql += "ORDER BY ti.channel_id, tsi.width DESC, tsi.fps DESC"; break;
    }

    m_queryModel->setQuerySql(sql, *m_dbManager->database());
}

void ManagerBackend::sortChannelTable(int column, int order)
{
    m_tableModel->setSort(column, order == 0 ? Qt::AscendingOrder : Qt::DescendingOrder);
    m_tableModel->select();
}

void ManagerBackend::resetConfig()
{
    //清空引导配置，下次启动时显示配置页面
    qInfo() << "Bootstrap path:" << m_bootstrapPath;
    if (!m_bootstrapPath.isEmpty()) {
        QSettings settings(m_bootstrapPath, QSettings::IniFormat);
        settings.setValue("paths/config", "");
        settings.setValue("paths/db", "");
        settings.sync();
        qInfo() << "Bootstrap config cleared";
        
        //验证清空结果
        QSettings verify(m_bootstrapPath, QSettings::IniFormat);
        qInfo() << "Verify config:" << verify.value("paths/config").toString();
        qInfo() << "Verify db:" << verify.value("paths/db").toString();
    }
    emit requestReconfigure();
}

void ManagerBackend::resetDatabase()
{
    m_dbManager->resetToDefault();
    refreshBrowseTable();
    refreshChannelTable();
}

void ManagerBackend::refreshBrowseTable()
{
    m_queryModel->setQuerySql(
        "SELECT ti.id, ti.channel_id, ti.name, ti.\"group\", tsi.ip, tsi.port, "
        "tsi.width, tsi.height, tsi.fps, st.name as type, tsi.notes "
        "FROM tv_info ti "
        "JOIN tv_source_info tsi ON ti.id = tsi.tv_id "
        "JOIN source_type st ON st.id = tsi.type "
        "ORDER BY ti.channel_id, tsi.width DESC, tsi.fps DESC",
        *m_dbManager->database());
}

void ManagerBackend::refreshChannelTable()
{
    m_tableModel->select();
}

void ManagerBackend::openScannerApp()
{
#ifdef Q_OS_WIN
    QProcess::startDetached("IPTVScan.exe");
#elif defined(Q_OS_MAC)
    QStringList searchPaths = {
        "/Applications/IPTVScan.app",
        QDir::homePath() + "/Applications/IPTVScan.app",
        QCoreApplication::applicationDirPath() + "/../../IPTVScan.app"
    };

    QString appPath;
    for (const QString &path : searchPaths) {
        if (QDir(path).exists()) {
            appPath = path;
            break;
        }
    }

    if (!appPath.isEmpty()) {
        QProcess::startDetached("open", {appPath});
    }
#endif
}

void ManagerBackend::onStreamProgress(int percent)
{
    m_checkProgress = percent;
    emit checkProgressChanged();
}

void ManagerBackend::onStreamSucceeded(const Iptv::Multimedia::StreamInfo &info, const QJsonObject &original)
{
    m_checkWidth = info.width;
    m_checkHeight = info.height;
    m_checkFps = info.fps;
    m_checkSourceType = info.sourceType;
    m_checkName = original.value("name").toString();
    m_checkNotes = original.value("notes").toString();
    m_checkProgress = 100;

    emit checkWidthChanged();
    emit checkHeightChanged();
    emit checkFpsChanged();
    emit checkSourceTypeChanged();
    emit checkNameChanged();
    emit checkNotesChanged();
    emit checkProgressChanged();
    emit valuesChangedSignal();

    //处理视频帧数据
    if (info.frameData && info.width > 0 && info.height > 0) {
        //将BGRA数据转换为QImage
        QImage image(info.frameData, info.width, info.height, info.width * 4, QImage::Format_ARGB32);
        m_framePixmap = QPixmap::fromImage(image.copy()); //copy()确保数据所有权
        if (m_frameImageProvider) {
            m_frameImageProvider->setFramePixmap(m_framePixmap);
        }
        emit framePixmapChanged();
    }

    //获取台标 - 使用数据库中的logo_name字段
    QString name = original.value("name").toString();
    if (!name.isEmpty()) {
        //从数据库获取logo_name
        QString logoName;
        if (m_channelRepo) {
            int channelId = m_channelRepo->findIdByName(name);
            if (channelId >= 0) {
                Database::TvChannel channel = m_channelRepo->findById(channelId);
                logoName = channel.logoName;
            }
        }

        //如果数据库中有logo_name，使用它；否则清空台标
        if (!logoName.isEmpty()) {
            QString logoBaseUrl = m_config->fileUrl();
            if (!logoBaseUrl.isEmpty()) {
                QString logoUrl = logoBaseUrl + "/" + logoName;
                m_logoFetcher->fetch(QUrl(logoUrl));
            }
        } else {
            //清空台标显示
            m_logoPixmap = QPixmap();
            if (m_logoImageProvider) {
                m_logoImageProvider->clearLogo();
            }
            emit logoPixmapChanged();
        }
    } else {
        //频道名称为空，清空台标显示
        m_logoPixmap = QPixmap();
        if (m_logoImageProvider) {
            m_logoImageProvider->clearLogo();
        }
        emit logoPixmapChanged();
    }

    emit checkingChanged();
}

void ManagerBackend::onStreamFailed(const QString &error)
{
    Q_UNUSED(error)
    emit checkingChanged();
}

void ManagerBackend::onLogoReady(const QPixmap &pixmap)
{
    m_logoPixmap = pixmap;
    if (m_logoImageProvider) {
        m_logoImageProvider->setLogoPixmap(pixmap);
    }
    emit logoPixmapChanged();
}

void ManagerBackend::onLogoFailed(const QString &error)
{
    Q_UNUSED(error)
    m_logoPixmap = QPixmap();
    if (m_logoImageProvider) {
        m_logoImageProvider->clearLogo();
    }
    emit logoPixmapChanged();
}

void ManagerBackend::onExportProgress(int percent)
{
    m_exportProgress = percent;
    emit exportProgressChanged();
}

void ManagerBackend::onExportFinished()
{
    m_exporting = false;
    emit exportingChanged();
}

void ManagerBackend::buildCheckUrl()
{
    if (m_checkIndex < 0 || m_checkIndex >= m_checkList.size()) return;

    QJsonObject source = m_checkList[m_checkIndex].toObject();
    //根据配置的协议类型（0=udp, 1=rtp）构建URL
    QString protocol = m_config->urlTypeIndex() == 1 ? "rtp" : "udp";
    m_currentCheckUrl = QString("%1/%2/%3:%4")
        .arg(m_config->httpUrl(),
             protocol,
             source.value("ip").toString())
        .arg(source.value("port").toInt());

    if (m_config->fccEnabled() && !m_config->fccUrl().isEmpty()) {
        m_currentCheckUrl += "?fcc=" + m_config->fccUrl();
    }
}

} // namespace Iptv::Ui

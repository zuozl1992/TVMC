#include "scan_service.h"
#include "multimedia/stream_probe.h"
#include "url_builder.h"

#include <QDebug>
#include <QCoreApplication>

namespace Iptv::Logic {

ScanService::ScanService(QObject *parent)
    : QObject(parent)
{
}

void ScanService::setUrlList(const QStringList &urls)
{
    m_urlList = urls;
}

void ScanService::setThreadMax(int max)
{
    m_threadMax = max;
}

void ScanService::setTimeout(int ms)
{
    m_timeout = ms;
}

void ScanService::setAutoStep(bool enabled)
{
    m_autoStep = enabled;
}

void ScanService::setSlowScan(bool enabled)
{
    m_slowScan = enabled;
}

void ScanService::start()
{
    if (isRunning())
        return;

    //重置计数器和状态
    m_pos = 0;
    m_okCount = 0;
    m_errorCount = 0;
    m_activeCount = 0;
    m_exitFlag = 0;
    m_ipCheck.clear();
    m_semaphore.release(m_threadMax);

    //在独立线程中处理URL列表，避免阻塞UI
    QThread::create([this]() {
        for (int i = 0; i < m_urlList.length(); i++) {
            if ((int)m_exitFlag)
                break;

            //获取信号量（控制并发数）
            m_semaphore.acquire();

            if ((int)m_exitFlag) {
                m_semaphore.release();
                break;
            }

            QString url = m_urlList.at(i);
            emit currentUrlChanged(url);

            //自动跳过模式：检查该IP是否已有成功记录
            if (m_autoStep) {
                QString ip = getIpFromUrl(url);
                bool step = m_ipCheck.value(ip, false);
                if (step) {
                    m_pos++;
                    m_errorCount++;
                    emit scanProgress((int)m_pos, m_urlList.length(),
                                     (int)m_okCount, (int)m_errorCount);
                    m_semaphore.release();
                    continue;
                }
            }

            //创建并启动探测器
            Multimedia::StreamProbe *probe = new Multimedia::StreamProbe();
            probe->setUrl(url);
            probe->setTimeout(m_timeout);
            probe->setSlowScan(m_slowScan);

            connect(probe, &Multimedia::StreamProbe::probeSucceeded,
                    this, &ScanService::onProbeSucceeded);
            connect(probe, &Multimedia::StreamProbe::probeFailed,
                    this, &ScanService::onProbeFailed);

            m_mutex.lock();
            m_probes.append(probe);
            m_mutex.unlock();

            m_activeCount++;
            probe->start();
        }

        //等待所有活跃探测器完成（检查退出标志）
        while ((int)m_activeCount > 0 && !(int)m_exitFlag) {
            QThread::msleep(10);
        }

        //如果是因为退出标志而结束，清理剩余探测器
        if ((int)m_exitFlag) {
            m_mutex.lock();
            for (auto *probe : m_probes) {
                probe->quit();
                probe->wait(500);
            }
            m_mutex.unlock();
        }

        emit scanFinished();
    })->start();
}

void ScanService::stop()
{
    //设置退出标志
    m_exitFlag = 1;

    //等待所有探测器退出
    m_mutex.lock();
    for (auto *probe : m_probes) {
        probe->quit();
        probe->wait(1000);
    }
    m_mutex.unlock();
}

bool ScanService::isRunning() const
{
    return (int)m_activeCount > 0;
}

void ScanService::onProbeSucceeded(const Multimedia::StreamInfo &info)
{
    Multimedia::StreamProbe *probe = qobject_cast<Multimedia::StreamProbe *>(sender());
    if (!probe)
        return;

    //记录成功IP（用于自动跳过）
    QString ip = getIpFromUrl(info.url);
    m_ipCheck.insert(ip, true);

    m_pos++;
    m_okCount++;

    emit scanResult(info);
    emit scanProgress((int)m_pos, m_urlList.length(),
                     (int)m_okCount, (int)m_errorCount);

    //清理探测器资源
    m_mutex.lock();
    m_probes.removeOne(probe);
    m_mutex.unlock();

    m_activeCount--;
    m_semaphore.release();

    probe->deleteLater();
}

void ScanService::onProbeFailed(const QString &error)
{
    Q_UNUSED(error)

    Multimedia::StreamProbe *probe = qobject_cast<Multimedia::StreamProbe *>(sender());
    if (!probe)
        return;

    m_pos++;
    m_errorCount++;

    emit scanProgress((int)m_pos, m_urlList.length(),
                     (int)m_okCount, (int)m_errorCount);

    //清理探测器资源
    m_mutex.lock();
    m_probes.removeOne(probe);
    m_mutex.unlock();

    m_activeCount--;
    m_semaphore.release();

    probe->deleteLater();
}

QString ScanService::getIpFromUrl(const QString &url)
{
    return UrlBuilder::getIpFromUrl(url);
}

} // namespace Iptv::Logic

#include "DownloadFile.h"
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qnetworkaccessmanager.h>
#include <qeventloop.h>
#include <qdir.h>
#include <qfile.h>

Download::Download(QString identify /*= ""*/)
{
    _identify = identify;
    _count = 0;
    _timeoutId = 0;
}

Download::~Download()
{
    if ( _file.isOpen() ) {
        _file.close();
    }
}
    
QUrl Download::redirectUrl(const QUrl& possibleRedirectUrl, const QUrl& oldRedirectUrl) const
{
    QUrl redirectUrl;
    if ( !possibleRedirectUrl.isEmpty() && possibleRedirectUrl != oldRedirectUrl ) {
        redirectUrl = possibleRedirectUrl;
    }
    return redirectUrl;
}

void Download::startRequest(QUrl url, QString filename)
{
    bool isRedirect = false;
    _filename = filename;
    _file.setFileName(filename);
    if ( QFile::exists(filename) ) {
        QFile::remove(filename);
    }
    _file.open(QIODevice::Append);
    QNetworkRequest request(url);
    QNetworkAccessManager manager;
    _downloadReply = manager.get(request);
    _downloadReply->setReadBufferSize(1024*1024*2);
    connect(_downloadReply, SIGNAL(readyRead()), this, SLOT(downloadReadyRead()));
    connect(_downloadReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(hasDownloaded(qint64, qint64)));
    QEventLoop waitLoop;
    _timeoutId = startTimer(5000);
    connect(_downloadReply, SIGNAL(finished()), &waitLoop, SLOT(quit()));
    waitLoop.exec();
    QVariant possibleRedirectUrl = _downloadReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    QUrl realUrl = redirectUrl(possibleRedirectUrl.toUrl(), url);
    if ( !realUrl.isEmpty() ) {
        isRedirect = true;
        url = realUrl;
    }
    _downloadReply->close();
    delete _downloadReply;
    _file.close();
    killTimer(_timeoutId);
    if ( isRedirect ) {
        startRequest(url, filename);
    }
}

void Download::timerEvent(QTimerEvent* t)
{
    
}
    
void Download::downloadReadyRead()
{
    _count = 0;
    if ( !_file.isOpen() ) {
        return;
    }
    _file.write(_downloadReply->readAll());
}
    
void Download::hasDownloaded(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(_identify, bytesReceived, bytesTotal);
}
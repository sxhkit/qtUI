#ifndef DOWNLOAD_FILE_H
#define DOWNLOAD_FILE_H

#include <qobject.h>
#include <qurl.h>
#include <qfile.h>
#include <qnetworkreply.h>
#include <qtimer.h>
#include <configuration.h>

class Download: public QObject {
    Q_OBJECT

public:
    Download(QString identify = "");
    ~Download();

    void startRequest(QUrl url, QString filename);

public slots:
    void downloadReadyRead();
    void hasDownloaded(qint64 bytesReceived, qint64 bytesTotal);
    void timerEvent(QTimerEvent* t);
signals:
    void downloadProgress(QString identify, qint64 bytesReceived, qint64 bytesTotal);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

protected:
    QUrl redirectUrl(const QUrl& possibleRedirectUrl, const QUrl& oldRedirectUrl) const;

private:
    QNetworkReply* _downloadReply;
    QString _filename;
    QString _identify;
    QFile   _file;
    //计数器，用来控制超时的变量,因为reply不能在下载中断网的情况下自己退出
    int _count;
    //超时定时器的id
    int _timeoutId;
};

#endif
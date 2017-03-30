#ifndef ADB_MAN_H
#define ADB_MAN_H

#include <stdint.h>
#include <qobject.h>
#include <qstring.h>
#include <qprocess.h>
#include <QTimerEvent>
#include "Common.h"
#include "configuration.h"

#define ADB_EXE "UMadb.exe"

class AdbMan: public QObject{
    Q_OBJECT

public:
    AdbMan();
    ~AdbMan();
    bool executeCommand(QString& serial, QString& cmd);
    bool terminalCommand();
    bool startSimulationClick(QString& serial);
    bool stopSimulationClick();
    bool setDefaultDesktop(QString& serial);
    bool setAuthority(QString& serial);
    bool pushFileToRemote(QString& serial, QString& file, QString dir = REMOTE_FILE_LOCATION);
    bool installInShellMode(QString& serial, QString& file, QString mode = "", int timeOut = 120);
    bool installToRemote(QString& serial, QString&file, int timeOut = 60);
    bool getAndroidRomFlag(QString& serial, QPair<QString, QString>& romPair, QVector<QVector<QString>> vecSys);
    bool getAndroidSysVersion(QString& serial, QString& sysVersion, QString& sysname);
    bool getDeviceModel(QString& serial, QString& deviceName);
    bool getAndroidBrandVersion(QString& serial, QString& brandVersion);
    bool getBrand(QString& serial, QString& brand);
    bool setTcpPortForward(QString& serial, QString from, QString to);
    bool checkTcpPortForward(QString& serial, QString from, QString to);
    bool setOppoAuthority(QString& serial);
    bool getAppVersion(QString& serial, QString appname, QString& version);
    bool getDeviceMemoryInfo(QString& serial, QMap<QString, QString>& memoryInfo);
    bool getDeviceSdCardInfo(QString& serial, QMap<QString, QString>& sdcardInfo);
    bool lightScreen(QString& serial);

    void setDisconnect()
    {
        disconnect(&_process, SIGNAL(readyRead()), this, SLOT(readOutput()));
    }

    QString getErrorString()
    {
        return _errorString;
    }

    QByteArray getOutput()
    {
        return _byteData;
    }

public slots:
    void readOutput();
protected:
    void clearSomething();

private:
    QProcess _process;
    QString _adbPath;
    QByteArray _byteData;
    QString _errorString;
};

#endif
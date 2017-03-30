#include "CheckDevices.h"
#include <qdir.h>
#include <qdebug.h>
#include <qstringlist.h>
#include <qobject.h>
#include "Common.h"

CheckDevices::CheckDevices()
{
    qRegisterMetaType<DeviceState>("DevciceState");
    connect(&_ipc, SIGNAL(hasGottenData(QByteArray&)), this, SLOT(processData(QByteArray&)));
    _heartBeatTimerId = startTimer(2000);
    _adb = NULL;
    startAdb();
}

CheckDevices::~CheckDevices()
{
    killTimer(_heartBeatTimerId);
    if ( _adb != NULL && _adb->state() == QProcess::ProcessState::Running ) {
        _adb->kill();
        qDebug() << _adb->errorString();
        delete _adb;
    }
    killAdb();
}

void CheckDevices::startAdb()
{
    
}

void CheckDevices::killOldVersionAsstProcesses()
{
    killAdb("UMengInstall.exe");
    killAdb("MobileMgrAdb.exe");
}

void CheckDevices::killAdb()
{
    QProcess process;
    process.execute("taskkill /fi \"imagename  eq  UMadb.exe\"  /f");
    process.waitForStarted();
    process.waitForFinished();
}

void CheckDevices::killAdb(QString processName)
{
    QProcess process;
    QString cmd;
    cmd = "taskkill /fi \"imagename  eq  ";
    cmd.append(processName).append("\"  /f");
    process.execute(cmd);
    process.waitForStarted();
    process.waitForFinished();
}

void CheckDevices::handleState(QProcess::ProcessState newState)
{
    switch ( newState )
    {
    case QProcess::NotRunning:
        startAdb();
        break;
    default:
        break;
    }
}
    
void CheckDevices::processData(QByteArray& datagram)
{
    _heartBeatTime.restart();
    QString s(datagram.data());
    parserConnectInfo(s);
}
    
void CheckDevices::parserConnectInfo(QString& str)
{
    
}
    
void CheckDevices::stateGo(QMap<QString, struct device_info>& deviceInfo)
{
    

}
    
void CheckDevices::timerEvent(QTimerEvent* event)
{
    if ( event->timerId() == _heartBeatTimerId ) {
        if ( _heartBeatTime.elapsed() >= 20000 ) {
            startAdb();
            _heartBeatTime.restart();
        }
    }
}
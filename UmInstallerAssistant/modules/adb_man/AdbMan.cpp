#include "AdbMan.h"
#include <qdir.h>
#include <qtextstream.h>
#include <qdebug.h>

static QMap<QString, QString> gAdbInstallErrorMap;

static void initErrorMap()
{
    static bool init = false;
    if ( !init ) {
//        gAdbInstallErrorMap["INSTALL_FAILED_ALREADY_EXISTS"] = "程序已存在";
//        gAdbInstallErrorMap["INSTALL_FAILED_INVALID_APK"] = "无效的安装包";
//        gAdbInstallErrorMap["INSTALL_FAILED_INVALID_URI"] = "无效的链接";
        gAdbInstallErrorMap["INSTALL_FAILED_INSUFFICIENT_STORAGE"] = "存储空间不足";/*"没有足够的存储空间";*/
        gAdbInstallErrorMap["No space left on device"] = "存储空间不足";
 //       gAdbInstallErrorMap["INSTALL_FAILED_DUPLICATE_PACKAGE"] = "已存在同名程序";
//        gAdbInstallErrorMap["INSTALL_FAILED_NO_SHARED_USER"] = "要求的共享用户不存在";
//        gAdbInstallErrorMap["INSTALL_FAILED_UPDATE_INCOMPATIBLE"] = "版本不能共存";
//        gAdbInstallErrorMap["INSTALL_FAILED_SHARED_USER_INCOMPATIBLE"] = "需求的共享用户签名错误";
//        gAdbInstallErrorMap["INSTALL_FAILED_MISSING_SHARED_LIBRARY"] = "需求的共享库已丢失";
//        gAdbInstallErrorMap["INSTALL_FAILED_REPLACE_COULDNT_DELETE"] = "需求的共享库无效";
//        gAdbInstallErrorMap["INSTALL_FAILED_DEXOPT"] = "dex优化验证失败";
        gAdbInstallErrorMap["INSTALL_FAILED_OLDER_SDK"] = "手机系统问题";/*"系统版本过旧";*/
//        gAdbInstallErrorMap["INSTALL_FAILED_CONFLICTING_PROVIDER"] = "存在同名的内容提供者";
        gAdbInstallErrorMap["INSTALL_FAILED_NEWER_SDK"] = "手机系统问题";/*"系统版本过新";*/
  //      gAdbInstallErrorMap["INSTALL_FAILED_TEST_ONLY"] = "调用不被允许测试的测试程序";
   //     gAdbInstallErrorMap["INSTALL_FAILED_CPU_ABI_INCOMPATIBLE"] = "包含的本机代码不兼容";
        //gAdbInstallErrorMap["CPU_ABIINSTALL_FAILED_MISSING_FEATURE"] = "使用了一个无效的特性";
        gAdbInstallErrorMap["INSTALL_FAILED_CONTAINER_ERROR"] = "SD卡访问失败";
//        gAdbInstallErrorMap["INSTALL_FAILED_INVALID_INSTALL_LOCATION"] = "无效的安装路径";
        gAdbInstallErrorMap["INSTALL_FAILED_MEDIA_UNAVAILABLE"] = "SD卡访问失败";
        gAdbInstallErrorMap["INSTALL_FAILED_INTERNAL_ERROR"] = "手机系统问题"/*"系统问题导致安装失败"*/;
        gAdbInstallErrorMap["INSTALL_FAILED_VERSION_DOWNGRADE"] = "无法覆盖安装";//"应用版本过低";
        gAdbInstallErrorMap["INSTALL_CANCELED_BY_USER"] = "安装权限未打开";
//        gAdbInstallErrorMap["INSTALL_PARSE_FAILED_INCONSISTENT_CERTIFICATES"] = "安装包签名不一致（卸载老版本后重试)";
 //       gAdbInstallErrorMap["INSTALL_FAILED_UPDATE_INCOMPATIBLE"] = "软件升级不兼容，请卸载后再安装";
    }
    init = true;
}

AdbMan::AdbMan()
{
    initErrorMap();
    QDir dir;
    _adbPath.append("\"");
    _adbPath.append(dir.currentPath());
    _adbPath.append("/");
    _adbPath.append(ADB_EXE);
    _adbPath.append("\"");
    connect(&_process, SIGNAL(readyRead()), this, SLOT(readOutput()));
    _process.setProcessChannelMode(QProcess::MergedChannels);
}

AdbMan::~AdbMan()
{
    _process.close();
    _process.kill();
}

bool AdbMan::executeCommand(QString& serial, QString& cmd)
{
    clearSomething();
    QString s;
    QTextStream(&s) << _adbPath << " -s " << serial << " " << cmd;
    _process.start(s);
    _process.waitForStarted();
    _process.waitForFinished();
    while ( _process.state() != QProcess::ProcessState::NotRunning ) {
        _process.close();
        _process.kill();
    }

    return true;
}

bool AdbMan::setAuthority(QString& serial)
{
    
    return false;
}
    
bool AdbMan::setOppoAuthority(QString& serial)
{
    
    return true;
}

bool AdbMan::startSimulationClick(QString& serial)
{
    return true;
}

bool AdbMan::stopSimulationClick()
{
    
    return true;
}

bool AdbMan::setDefaultDesktop(QString& serial)
{
    
    return false;
}

bool AdbMan::pushFileToRemote(QString& serial, QString& file, QString dir /* = DEFAULT_PUSHFILE_DIR*/)
{
    
    return false;
}

bool AdbMan::installInShellMode(QString& serial, QString& file, QString mode, int timeOut /* = 30 */)
{
    
    return false;
}

bool AdbMan::installToRemote(QString& serial, QString&file, int timeOut)
{
   
    return false;
}

bool AdbMan::getAndroidRomFlag(QString& serial, QPair<QString, QString>& romPair, QVector<QVector<QString>> vecSys)
{
   
    return false;
}

bool AdbMan::getBrand(QString& serial, QString& brand)
{
    
    return true;
}

bool AdbMan::getAndroidSysVersion(QString& serial, QString& sysVersion, QString& sysname)
{
    

    return true;
}

bool AdbMan::getDeviceModel(QString& serial, QString& deviceName)
{
    clearSomething();
    QString cmd;
    QTextStream(&cmd) << _adbPath << " -s " << serial << " shell getprop";
    _process.start(cmd);
    if ( !_process.waitForStarted() ) {
        return false;
    }
    _process.waitForFinished();
    while ( _process.state() != QProcess::ProcessState::NotRunning ) {
        _process.close();
        _process.kill();
    }
    QRegExp r;
    QString pattern = QRegExp::escape("[ro.product.model]: [") + "([^\\]]+)" + QRegExp::escape("]");
    r.setPattern(pattern);
    if ( r.indexIn(_byteData) == -1 ) {
        return false;
    }
    deviceName = r.cap(1);
    return true;
}

bool AdbMan::getAndroidBrandVersion(QString& serial, QString& brandVersion)
{
    
    return true;
}
    
bool AdbMan::getAppVersion(QString& serial, QString appname, QString& version)
{
   
    return true;
}

bool AdbMan::setTcpPortForward(QString& serial, QString from, QString to)
{
    
    return true;
}
    
bool AdbMan::checkTcpPortForward(QString& serial, QString from, QString to)
{
  
    return true;
}
    
bool AdbMan::getDeviceMemoryInfo(QString& serial, QMap<QString, QString>& memoryInfo)
{
    
    return true;
}
    
bool AdbMan::getDeviceSdCardInfo(QString& serial, QMap<QString, QString>& sdcardInfo)
{
    
    return true;
}
    
bool AdbMan::lightScreen(QString& serial)
{
    
    return true;
}

void AdbMan::readOutput()
{
   
}

void AdbMan::clearSomething()
{
    
}


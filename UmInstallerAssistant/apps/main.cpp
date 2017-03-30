#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "UmInstallerAssistant.h"
#include <QtWidgets/QApplication>
#include "CheckDevices.h"
#include <qstring.h>
#include "Ipc.h"
#include "Common.h"
#include <wtypes.h>

int main(int argc, char *argv[])
{
    int ret = 0;
    HANDLE hMutex = CreateMutexA(NULL, FALSE, "UMInstallerAssistant");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(hMutex);
        return -1;
    }
    qInstallMessageHandler(Common::logMessage);
    QApplication a(argc, argv);
    UmInstallerAssistant w(&a);
    w.show();
    ret = a.exec();
    CloseHandle(hMutex);
    return ret;
}

//  Copyright (C)  WSWY
//
//  File:       Common.h
//
//  History:    2017-Jan-1-22   Xhsund  Created
//
//--------------------------------------------------------------------------
#ifndef UMINSTALLERASSISTANT_INCLUDE_COMMON_H_
#define  UMINSTALLERASSISTANT_INCLUDE_COMMON_H_
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include "QtCore"

#define  MAX_CONNECT_MOBILE_NUM   5
#define  SAFEDELETE(x)   do { \
  if ( (x) != NULL) {\
    delete (x); \
     (x) = NULL;}\
}while (false)

class Common {
 public:
    static bool isIconFileExist(QString icon);
    static QString getIconSavePath(QString url);
    static QString readStyleSheet(QString fileName);
    static int a2hex(QString s);
    static QStringList qstr2QstrList(QString src, QString separator);
    static bool isNetConnected();
    static QString getImageFormat(QString imagePath);
    static void logMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};
#endif  // UMINSTALLERASSISTANT_INCLUDE_COMMON_H_

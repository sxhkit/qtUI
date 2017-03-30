//  Copyright (C)  WSWY
//
//  File:       Common.cpp
//
//  History:    2017-Jan-1-22   Xhsund  Created
//
//--------------------------------------------------------------------------
#include "Common.h"
#include <qfile.h>
#include <qhostinfo.h>
#include <qtcpsocket.h>
#include <qnetworkconfigmanager.h>
#include "qpixmap.h"

bool Common::isIconFileExist(QString url) {
    QString icon = getIconSavePath(url);
    QFile fd(icon);
    if (fd.exists()) {
        return true;
    }
    return false;
}

QString Common::getIconSavePath(QString url) {
    QString savePath = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);
    savePath.append("/NetImage/");
    QDir dir;
    if (!dir.exists(savePath)) {
        dir.mkpath(savePath);
    }
    int n = url.lastIndexOf("/");
    QString filename = url.mid(n + 1);
    savePath.append(filename);
    return savePath;
}

QString Common::readStyleSheet(QString fileName) {
    QFile file(fileName);
    QString data;
    if (file.exists()) {
        file.open(QFile::ReadOnly);
        data = QString::fromLatin1(file.readAll());
    }
    return data;
}

int qcharToInt(QChar c) {
    if ( c >= QChar('0') && c <= QChar('9') ) {
        return c.toLatin1() - '0';
    } else if ( c >= QChar('a') && c <= QChar('f') ) {
        return c.toLatin1() - 'a' + 10;
    } else if ( c >= QChar('A') && c <= QChar('F') ) {
        return c.toLatin1() - 'A' + 10;
    } else {
        return 0;
    }
}

int Common::a2hex(QString s) {
    int sum = 0;
    for ( int i = 0; i < s.length() ; i++ ) {
        sum = 16 * sum + qcharToInt(s.at(i));
    }
    return sum;
}

QStringList Common::qstr2QstrList(QString src, QString separator) {
    QStringList empty;
    if (src.isEmpty()) return empty;
    empty = src.split(separator);
    return empty;
}

bool Common::isNetConnected() {
    /*
    bool ret = false;
    QTcpSocket tcpClient;
    tcpClient.connectToHost("shouji.youmeng.com", 80);
    if (tcpClient.waitForConnected(300))
        ret = true;
    else
        ret = false;
    return ret;
    */
    //ͨ���ж�����״̬���ж������Ƿ�Ͽ�������û�п����������������
    QNetworkConfigurationManager mgr;
    QList<QNetworkConfiguration> activeConfigs = mgr.allConfigurations(QNetworkConfiguration::Active);
    return (activeConfigs.count() > 0);
}

#define BMP_FORMAT  "BMP"
#define JPG_FORMAT  "JPG"
#define PNG_FORMAT  "PNG"
#define GIF_FORMAT  "GIF"
#define NVL_FORMAT  "ERR"

QString Common::getImageFormat(QString imagePath)
{
    QString format = NVL_FORMAT;
    QPixmap pix(imagePath);
    if (pix.isNull()) {
        //BMP��ʽ������
        unsigned char BMPHeader[] = { 0x42, 0x4d };
        //JPG,JPEG��ʽ������
        unsigned char JPGHeader1[] = { 0xff, 0xd8, 0xff, 0xdb };
        unsigned char JPGHeader2[] = { 0xff, 0xd8, 0xff, 0xe0 };
        unsigned char JPGHeader3[] = { 0xff, 0xd8, 0xff, 0xe1 };
        unsigned char JPGHeader4[] = { 0xff, 0xd8, 0xff, 0xe2 };
        unsigned char JPGHeader5[] = { 0xff, 0xd8, 0xff, 0xe3 };
        unsigned char JPGHeader6[] = { 0xff, 0xd8, 0xff, 0xe8 };
        //GIF��ʽ������
        unsigned char GIFHeader1[] = { 0x47, 0x49, 0x46, 0x38, 0x37, 0x61 };
        unsigned char GIFHeader2[] = { 0x47, 0x49, 0x46, 0x38, 0x39, 0x61 };
        //PNG��ʽ������
        unsigned char PNGHeader[] = { 0x89, 0x50, 0x4E, 0x47 };
        int count = 0;
        int step = 2;
        //�Զ����Ʒ�ʽ���ļ�����ȡǰ�����ֽ�
        unsigned char header[16];
        QFile  f(imagePath);
        if (!f.open(QIODevice::ReadOnly))
            return NVL_FORMAT;
        QDataStream stream(&f);
        do {
            for (int i = 0; i < step; i++) {
                stream >> header[count + i];
            }
            count = count + step;
            if (memcmp(header, BMPHeader, count) == 0) {
                format = BMP_FORMAT;
            }
            //�ٶ��������ж��Ƿ�JPG��ʽ��PNG��ʽ
            for (int i = 0; i < step; i++) {
                stream >> header[count + i];
            }
            count = count + step;
            if ((memcmp(header, JPGHeader1, count) == 0)
                || (memcmp(header, JPGHeader2, count) == 0)
                || (memcmp(header, JPGHeader3, count) == 0)
                || (memcmp(header, JPGHeader4, count) == 0)
                || (memcmp(header, JPGHeader5, count) == 0)
                || (memcmp(header, JPGHeader6, count) == 0)) {
                format = JPG_FORMAT;
                break;
            } else if (memcmp(header, PNGHeader, count) == 0) {
                format = PNG_FORMAT;
                break;
            }
            //�ٶ��������ж��Ƿ�GIF��ʽ
            for (int i = 0; i < step; i++) {
                stream >> header[count + i];
            }
            count = count + step;
            if ((memcmp(header, GIFHeader1, count) == 0)
                || (memcmp(header, GIFHeader2, count) == 0)) {
                format = GIF_FORMAT;
                break;
            }
            qDebug() << "�ļ���ʽ������:";
            for (int i = 0; i < count; i++) {
                printf("%0x\t", header[i]);
            }
            printf("\n");
            qDebug() << "�����������κ�һ�ָ�ʽ";
            format = NVL_FORMAT;
            break;
        } while (false);
    }
    return format;
}

void Common::logMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
#ifndef _DEBUG
    if ( type == QtDebugMsg ) {
        fprintf(stdout, "%s:%u, %s: %s\n", context.file, context.line, context.function, msg); 
        return;
    }
#endif
    static QMutex mutex;
    mutex.lock();
    static QString filename = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" +  "UMINSTALLER" + QString(QDateTime::currentDateTime().toLocalTime().toString("yyyyMMddhhmmss")) + ".log";
    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    if ( file.isOpen() ) {
        QTextStream textStream(&file);
        textStream.setCodec("UTF-8");
#ifdef _DEBUG
        textStream << context.file << ":" << context.line << " " << context.function << " " << msg << "\n";
#else
        textStream << msg << "\n";
#endif
        file.flush();
        file.close();    
    }
    mutex.unlock();
}

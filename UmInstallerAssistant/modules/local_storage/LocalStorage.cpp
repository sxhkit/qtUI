#include "LocalStorage.h"
#include <qdir.h>
#include <qdatastream.h>
#include <qstandardpaths.h>
#include <qvector.h>

LocalStorage::LocalStorage()
{
    QDir dir; 
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    path.append("/data");
    if ( !dir.exists(path) ) {
        dir.mkpath(path);
    }
    _dataPath = path;
    _dataPath.append("/polaris.dat");
}

LocalStorage::~LocalStorage()
{
    _dataPath.clear();
}
    
void LocalStorage::cleanData()
{
    _mutex.lock();
    QFile::remove(_dataPath);
    _mutex.unlock();
}

void LocalStorage::saveUploadData(const QByteArray& data)
{
    _mutex.lock();
    QString filename = _dataPath;
    QFile file(filename);
    file.open(QIODevice::Append);
    QDataStream out(&file);
    QByteArray save;
    quint16 crc = qChecksum(data.data(), data.length());
    out << crc;
    save = data.toBase64();
    out << save;
    file.close();
    _mutex.unlock();
}
    
void LocalStorage::saveUploadData(QVector<QByteArray>& data)
{
    _mutex.lock();
    QVector<int> b;
    QString filename = _dataPath;
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QDataStream out(&file);
    for ( auto it = data.begin(); it != data.end(); it++ ) {
        quint16 crc = qChecksum(it->data(), it->length());
        out << crc;
        QByteArray save = it->toBase64();
        out << save;
    }
    file.close();
    _mutex.unlock();
}

void LocalStorage::loadUploadData(QVector<QByteArray>& data)
{
    _mutex.lock();
    QString filename = _dataPath;
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    while ( !in.atEnd() ) {
        quint16 crc;
        in >> crc;
        QByteArray form;
        in >> form; 
        QByteArray trans = QByteArray::fromBase64(form);
        if ( qChecksum(trans.data(), trans.length()) == crc ) {
            data.push_back(trans);
        }
    }
    file.close();
    _mutex.unlock();
}


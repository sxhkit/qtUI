#ifndef IPC_H
#define IPC_H

#include <qtcpserver.h>
#include <qobject.h>
#include <qtcpsocket.h>
#include <qudpsocket.h>


class TcpIpc: public QObject{
    Q_OBJECT

    TcpIpc();
    ~TcpIpc();

private:
    QTcpServer _tcpServer;
    QTcpSocket _socket;
};

class UdpIpc: public QObject{

    Q_OBJECT
        
public:
    UdpIpc(short port = 7689)
    {
        if ( _udpSocket.state() != _udpSocket.BoundState ) {
            if (!_udpSocket.bind(QHostAddress::LocalHost, port) ) {
                qDebug() << "bind error" ;
            }
        }
        connect(&_udpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
    }

    ~UdpIpc()
    {
        _udpSocket.close();
    }

public slots:
    void readData(); 

signals:
    void hasGottenData(QByteArray& data);
private:
    QUdpSocket _udpSocket;
};

class TcpClient: public QObject {
    Q_OBJECT
public:
    TcpClient(const QString& hostname, quint16 port);
    ~TcpClient();
    bool connectToHost();
    void read(QByteArray& data);
    void write(QByteArray& data);
public slots:
    void hasConnected();
    void hasDisconnected();
    void hasReadyRead();
    void hasErrorOccur(QAbstractSocket::SocketError error);
signals:
    void connected();
    void disconnected();
    void isReadyRead();
    void errorOccur(QAbstractSocket::SocketError error);
    
private:
    QTcpSocket _socket;
    QString _host;
    quint16 _port;
    QByteArray _buffer;
    bool _isAvalliable;
};

#endif

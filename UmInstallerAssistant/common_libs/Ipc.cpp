#include "Ipc.h"

void UdpIpc::readData()
{
    while ( _udpSocket.hasPendingDatagrams() ) {
        QByteArray datagram;
        datagram.resize(_udpSocket.pendingDatagramSize());
        QHostAddress hostAddr;
        quint16 senderPort;
        _udpSocket.readDatagram(datagram.data(), datagram.size(), &hostAddr, &senderPort);
        emit hasGottenData(datagram);
    }
}

TcpClient::TcpClient(const QString& hostname, quint16 port):_host(hostname), _port(port)
{
    qRegisterMetaType<QAbstractSocket::SocketError>("SockerError");
    connect(&_socket, SIGNAL(connected()), this, SLOT(hasConnected()));
    connect(&_socket, SIGNAL(disconnected()), this, SLOT(hasDisconnected()));
    connect(&_socket, SIGNAL(readyRead()), this, SLOT(hasReadyRead()));
    connect(&_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(hasErrorOccur(QAbstractSocket::SocketError)));
    _isAvalliable = false;
}

TcpClient::~TcpClient()
{
    if ( _socket.isOpen() ) {
        _socket.close();
    }
}
    
bool TcpClient::connectToHost()
{
    _socket.connectToHost(_host, _port);
    return true;
}
    
void TcpClient::hasErrorOccur(QAbstractSocket::SocketError error)
{
    emit errorOccur(error);
}

void TcpClient::hasConnected()
{
    _isAvalliable = true;
    emit connected();
}

void TcpClient::hasDisconnected()
{
    _isAvalliable = false;
    emit disconnected();
}
    
void TcpClient::hasReadyRead()
{
    _buffer = _socket.read(_socket.bytesAvailable());
    emit isReadyRead();
}
    
void TcpClient::read(QByteArray& data)
{
    data = _buffer;
}
    
void TcpClient::write(QByteArray& data)
{
    if ( _isAvalliable ) {
        _socket.write(data);
    }
}

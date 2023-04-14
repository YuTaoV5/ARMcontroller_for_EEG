#include "tcpclient.h"
TcpClient::TcpClient()
{
    socket = new QTcpSocket;
    serverIP.setAddress(ip);
    this->socket->connectToHost(serverIP,port);
}

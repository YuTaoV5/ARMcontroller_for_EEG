#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QTcpSocket>
#include <QHostAddress>

class TcpClient : public QTcpSocket
{
    Q_OBJECT
public:
    TcpClient();
    ~TcpClient();
    QTcpSocket *socket;
private:
    QHostAddress serverIP;
    unsigned short int port = 81;    //esp32-cam 视频流端口
    QString ip = "192.168.132.126";  //ESP32-cam 分配到的IP
};

#endif // TCPCLIENT_H

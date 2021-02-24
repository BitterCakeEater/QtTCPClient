#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>

class TCPClient : public QObject
{
    Q_OBJECT

public:
    TCPClient();
    ~TCPClient();

    QString line;

    QString userName;
    QMap<QString, QStringList> messageStorage;//(user name, message log)

    QTcpSocket* socket;
    QByteArray Data;

    bool connected;//if socket is connected to server
    int serverStatus;

signals:
    void sready();
    void sdisconnect();
    void sconnected();

public slots:
    void socketReady();

    void socketDisconnect();

    void socketConnected();

    void Send(QString reciever, QString text);

    void Login(QString ServerIP);

    void LogOut();
};

#endif // TCPCLIENT_H

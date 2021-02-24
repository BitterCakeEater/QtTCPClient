#include "tcpclient.h"

TCPClient::TCPClient()
{
    connected = false;
    serverStatus = 0;
}

TCPClient::~TCPClient()
{

}

void TCPClient::socketDisconnect()
{
    serverStatus = 0;
    connected = false;
    socket->deleteLater();

    emit sdisconnect();
}

void TCPClient::socketConnected()
{
    emit sconnected();
}

void TCPClient::socketReady()
{
    while(socket->canReadLine())
    {
        line = QString::fromUtf8(socket->readAll()).trimmed();

        qDebug() << "Input: " << line;

        //If users list
        if(line[0] == '1')
        {
            QStringList sList = line.split(",");
            sList.removeFirst();

            QStringList messageList;
            messageList.append("");

            foreach(QString s, sList)
            {
                messageStorage[s] = messageList;
            }

            serverStatus = 2;
        }

        //If normal chat message:
        else if(line[0] == '2')
        {
            QStringList sList = line.split(" ");

            QString userFrom = sList[1];
            QString userTo = sList[2];

            int pos = 2 + userFrom.length() + userTo.length() + 1;

            QString message = line.mid(pos, line.length() - pos);

            qDebug() << "Get: " << message;

            if(userFrom == userTo == userName)
            {
                if(messageStorage[userName][0] == "")
                    messageStorage[userName].removeAt(0);

                messageStorage[userName].append("<b>" + userFrom + "</b>: " + message);
                qDebug() << "Get: " << messageStorage[userName];
            }
            else
            {
                if(userTo == userName)
                {
                    if(messageStorage[userFrom][0] == "")
                        messageStorage[userFrom].removeAt(0);

                    messageStorage[userFrom].append("<b>" + userFrom + "</b>: " + message);
                }
                else
                {
                    if(messageStorage[userTo][0] == "")
                        messageStorage[userTo].removeAt(0);

                    messageStorage[userTo].append("<b>" + userFrom + "</b>: " + message);
                }
            }
        }

        //If new user:
        else if(line[0] == '4')
        {
            QStringList messageList;
            messageList.append("");

            messageStorage[line.mid(2, line.length() - 2)] = messageList;

            qDebug() << "New client " << line.mid(2, line.length() - 2);
        }

        //Server status change:
        else if(line[0] == '5')
        {
            if(line[2] == '1')
            {
                serverStatus = 1;
            }
            else if(line[2] == '2')
            {
                serverStatus = 2;
            }
        }

        //Delete user:
        else if(line[0] == '0')
        {
            qDebug() << "Deletion " << line.mid(2);

            messageStorage.remove(line.mid(2));
        }
    }

    emit sready();
}

void TCPClient::Send(QString reciever, QString text)
{
    if(connected && serverStatus == 2)
    {
        if(!text.isEmpty())
            socket->write(QString("2 "+ userName + " " + reciever + " " + text  + '\n').toUtf8());

        qDebug() << "Sent: " << QString("2 "+ userName + " " + reciever + " " + text);
    }
}

void TCPClient::Login(QString ServerIP)
{
    if(!connected)
    {
        socket = new QTcpSocket(this);
        connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnect()));
        connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));

        socket->connectToHost(ServerIP, 123);//"192.168.0.105"

        socket->waitForConnected(500);

        if(socket->state() != QTcpSocket::ConnectedState)
        {
            this->socketDisconnect();
        }

        else
            connected = true;
    }
}

void TCPClient::LogOut()
{
    connected = false;
    qDebug() << "2f";
    if(socket->state() == QTcpSocket::ConnectedState)
    {
        qDebug() << "3f";
        socket->close();
        qDebug() << "4f";
        socket->deleteLater();
    }
    messageStorage.clear();
    qDebug() << "10f";
}

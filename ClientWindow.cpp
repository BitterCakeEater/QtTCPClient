#include "ClientWindow.h"
#include "ui_ClientWindow.h"

#include <QMessageBox>

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Client = new TCPClient();

    connect(Client, SIGNAL(sready()), this, SLOT(socketReady()));
    connect(Client, SIGNAL(sdisconnect()), this, SLOT(socketDisconnect()));
    connect(Client, SIGNAL(sconnected()), this, SLOT(socketConnected()));

    ui->stackedWidget->setCurrentIndex(0);
}

ClientWindow::~ClientWindow()
{
    delete ui;
}

void ClientWindow::socketReady()
{    
    QString line = Client->line;

    //If users list
    if(line[0] == '1')
    {
        QStringList sList = line.split(",");
        sList.removeFirst();

        QStringList messageList;
        messageList.append("");

        foreach(QString s, sList)
        {
            new QListWidgetItem(s, ui->listWidget_Clients);
        }

        ui->stackedWidget->setCurrentIndex(1);
        ui->label_ID->setText(Client->userName);
        ui->label_serverStatus->setText("Server status: On");
    }

    //If normal chat message:
    else if(line[0] == '2')
    {
        QStringList sList = line.split(" ");

        QString userFrom = sList[1];
        QString userTo = sList[2];

        int pos = 2 + userFrom.length() + userTo.length() + 1;

        QString message = line.mid(pos, line.length() - pos);

        if(userFrom == userTo == Client->userName)
        {
            if(ui->listWidget_Clients->selectedItems().count() > 0)
            {
                if(ui->listWidget_Clients->currentItem()->text() == Client->userName)
                    //ui->textEdit_ChatWindow->append("<b>" + userFrom + "</b>: " + message);
                    writeMessage(userFrom + ":", message);

                else
                    ui->listWidget_Clients->findItems(userFrom, Qt::MatchExactly)[0]->setBackgroundColor(Qt::cyan);
            }
            else
                ui->listWidget_Clients->findItems(Client->userName, Qt::MatchExactly)[0]->setBackgroundColor(Qt::cyan);
        }
        else
        {
            if(userTo == Client->userName)
            {
                if(ui->listWidget_Clients->selectedItems().count() > 0)
                {
                    if(ui->listWidget_Clients->currentItem()->text() == userFrom)
                        //ui->textEdit_ChatWindow->append("<b>" + userFrom + "</b>: " + message);
                        writeMessage(userFrom + ":", message);

                    else
                        ui->listWidget_Clients->findItems(userFrom, Qt::MatchExactly)[0]->setBackgroundColor(Qt::cyan);
                }
                else
                    ui->listWidget_Clients->findItems(userFrom, Qt::MatchExactly)[0]->setBackgroundColor(Qt::cyan);
            }
            else
            {
                if(ui->listWidget_Clients->selectedItems().count() > 0)
                {
                    if(ui->listWidget_Clients->selectedItems()[0]->text() == userTo)
                        //ui->textEdit_ChatWindow->append("<b>" + userFrom + "</b>: " + message);
                        writeMessage(userFrom + ":", message);

                    else
                       ui->listWidget_Clients->findItems(userTo, Qt::MatchExactly)[0]->setBackgroundColor(Qt::cyan);
                }
                else
                   ui->listWidget_Clients->findItems(userTo, Qt::MatchExactly)[0]->setBackgroundColor(Qt::cyan);
            }
        }
    }

    //If login is taken:
    else if(line[0] == '3')
    {
        ui->lineEdit_userName->clear();
        ui->lineEdit_userName->setPlaceholderText("This login is taken");
    }

    //If new user:
    else if(line[0] == '4')
    {
        QStringList messageList;
        messageList.append("");

        new QListWidgetItem(line.mid(2, line.length() - 2), ui->listWidget_Clients);
    }

    //Server status change:
    else if(line[0] == '5')
    {
        if(line[2] == '1')
        {
            ui->label_serverStatus->setText("Server status: Paused");
        }
        else if(line[2] == '2')
        {
            ui->label_serverStatus->setText("Server status: On");
        }
    }

    //Delete user:
    else if(line[0] == '0')
    {
        if(ui->listWidget_Clients->selectedItems().count() > 0)
            if(ui->listWidget_Clients->currentItem()->text() == line.mid(2))
                ui->textEdit_ChatWindow->clear();

        foreach(QListWidgetItem* s, ui->listWidget_Clients->findItems(line.mid(2), Qt::MatchExactly))
        {
            if(s->text() == line.mid(2))
                ui->listWidget_Clients->takeItem(ui->listWidget_Clients->row(s));
        }
    }
}

void ClientWindow::socketDisconnect()
{
    ui->label_serverStatus->setText("Server status: Off");
    //ui->listWidget_Clients->clear();
}

void ClientWindow::socketConnected()
{
    Client->userName = ui->lineEdit_userName->text().trimmed();
    Client->socket->write(QString("1 " + ui->lineEdit_userName->text().trimmed() + '\n').toUtf8());

    Client->socket->waitForReadyRead();
    QString input;
    input = Client->socket->readAll();

    if(input[0] == '3')
    {
        ui->lineEdit_userName->clear();
        ui->lineEdit_userName->setPlaceholderText("This login is taken");
    }
}

void ClientWindow::on_pushButton_Send_clicked()
{
    if(Client->connected && (ui->listWidget_Clients->selectedItems().length() > 0) && Client->serverStatus == 2)
    {
        QString userTo = ui->listWidget_Clients->selectedItems()[0]->text();

        QString text;

        text = ui->textEdit_Input->toPlainText().trimmed();

        Client->Send(userTo, text);

        ui->textEdit_Input->clear();
        ui->textEdit_Input->setFocus();
    }
}

void ClientWindow::on_pushButton_Login_clicked()
{
    if(ui->lineEdit_userName->text().trimmed().length() > 0 && ui->lineEdit_serverIP->text().trimmed().length()){
        if(!Client->connected)
        {
            Client->Login(ui->lineEdit_serverIP->text().trimmed());

            if(Client->socket->state() != QTcpSocket::ConnectedState)
            {
                QMessageBox::warning(this, "Warning!","Unable to connect to server");
            }
        }
        else
        {
            Client->userName = ui->lineEdit_userName->text().trimmed();
            Client->socket->write(QString("1 " + ui->lineEdit_userName->text().trimmed() + '\n').toUtf8());

            Client->socket->waitForReadyRead();
            QString input;
            input = Client->socket->readAll();

            if(input[0] == '3')
            {
                ui->lineEdit_userName->clear();
                ui->lineEdit_userName->setPlaceholderText("This login is taken");
            }
        }
    }
    else
    {
        ui->lineEdit_userName->clear();
        ui->lineEdit_userName->setPlaceholderText("Type your login...");
        ui->lineEdit_serverIP->setPlaceholderText("Type server IP...");
    }
}

void ClientWindow::on_pushButton_LogOut_clicked()
{
    qDebug() << "1";
    ui->stackedWidget->setCurrentIndex(0);

    Client->LogOut();

    qDebug() << "2";
    ui->label_ID->setText("");
    ui->label_chat->setText("");
    qDebug() << "6";
    ui->listWidget_Clients->clear();
    qDebug() << "7";
    ui->textEdit_ChatWindow->clear();
    qDebug() << "8";
    ui->textEdit_Input->clear();
    qDebug() << "9";
}

void ClientWindow::on_listWidget_Clients_itemClicked(QListWidgetItem *item)
{
    item->setBackgroundColor(Qt::white);

    ui->label_chat->setText("Chat with " + item->text());

    ui->textEdit_ChatWindow->clear();

    foreach(QString s, Client->messageStorage[item->text()])
    {
        writeMessage(s.split(" ").first(), s.mid(s.split(" ").first().length()));
        //ui->textEdit_ChatWindow->append(s);
    }
}

void ClientWindow::writeMessage(QString userFrom, QString message)
{
    QStringList sList = message.split('\n');

    ui->textEdit_ChatWindow->append("<b>" + userFrom + "</b> " + sList[0]);

    for(int i = 1;i < sList.size();i++)
    {
        ui->textEdit_ChatWindow->append(sList[i]);
    }
}

#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include "tcpclient.h"

#include <QMainWindow>
#include <QTcpSocket>
#include <QDebug>
#include <QListWidgetItem>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

public slots:
    void socketReady();

    void socketDisconnect();

    void socketConnected();

private slots:
    void on_pushButton_Send_clicked();

    void on_pushButton_Login_clicked();

    void on_pushButton_LogOut_clicked();

    void on_listWidget_Clients_itemClicked(QListWidgetItem *item);

    void writeMessage(QString userFrom, QString message);

private:
    Ui::MainWindow *ui;

    TCPClient *Client;
};
#endif // TCPCLIENT_H

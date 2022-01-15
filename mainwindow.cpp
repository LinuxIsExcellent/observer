#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "parseconfig.h"
#include "globalconfig.h"
#include <QSettings>
#include <QDebug>
#include <QMessageBox>

#include "msg.pb.h"
#include "log.h"
#include <google/protobuf/text_format.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_ServerSockect = new QTcpSocket(this);

    //连接事件
    connect(m_ServerSockect,&QTcpSocket::connected, this, &MainWindow::OnServerConnect);
    //有可读事件
    connect(m_ServerSockect,&QTcpSocket::readyRead, this, &MainWindow::OnServerMsgRecv);
    //socket错误事件
    connect(m_ServerSockect, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnSocketError(QAbstractSocket::SocketError)));

    //点击连接服务器
    connect(ui->connect_server, SIGNAL(clicked()), this, SLOT(OnClickConnectServerBtn()));

    connect(ui->comboBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(OnShowProgramServerLists(const QString &)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_windows()
{
    ui->comboBox->clear();

    const qMapPrograms* mapProgram = GlobalConfig::getInstance()->GetProgramData();
    if(mapProgram)
    {
        for (qMapPrograms::const_iterator it = mapProgram->begin();it != mapProgram->end() ; ++it) {
            ui->comboBox->addItem(it.value().sProgramName, it.key());
        }
    }


    OnShowProgramServerLists();
}

void MainWindow::OnShowProgramServerLists(const QString &)
{
    QString programName = ui->comboBox->currentData().toString();
    qDebug() << programName;

    const qMapPrograms* mapProgram = GlobalConfig::getInstance()->GetProgramData();
    if (mapProgram)
    {
        if(mapProgram->contains(programName))
        {
            sProgram program = mapProgram->value(programName);
            for (QVector<sServerInfo>::const_iterator it = program.serverList.begin();it != program.serverList.end() ; ++it)
            {
                it->name;
                it->ip;
                it->port;
            }
        }
    }
//    ui->listWidget->addItems(list);
}

//请求连接到特定服务器
void MainWindow::OnClickConnectServerBtn()
{
    QString ip = "192.168.184.130";
    uint16_t port = 23543;

    m_ServerSockect->connectToHost(QHostAddress(ip),port);
}

//socket错误码
void MainWindow::OnSocketError(QAbstractSocket::SocketError error)
{
    QString str;
    if (error == QAbstractSocket::ConnectionRefusedError)
    {
        str = "服务器拒绝连接！！！";
    }
    else if (error == QAbstractSocket::OperationError)
    {
        str = "重复请求连接！！！";
    }
    else if (error == QAbstractSocket::RemoteHostClosedError)
    {
        str = "服务器断开连接！！！";
    }

    QMessageBox information(QMessageBox::Critical, tr("警告"), str, QMessageBox::Ok);
    information.exec();
    return;
}

//服务器连接成功
void MainWindow::OnServerConnect()
{
    qDebug () << "服务器连接成功";
}

void MainWindow::OnServerMsgRecv()
{
    QByteArray data = m_ServerSockect->readAll();
    QString s = QString(data);
    qDebug() << "recv:" << s;
}

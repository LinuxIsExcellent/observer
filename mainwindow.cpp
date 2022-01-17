#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "parseconfig.h"
#include "globalconfig.h"
#include <QSettings>
#include <QDebug>
#include <QMessageBox>

#include "msg.pb.h"
#include "log.h"
#include "Packet.h"
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
    m_ServerSockect->close();
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
    QString ip = "192.168.184.250";
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
        m_ServerSockect->close();
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

/*发送数据包
 * in nSystem：系统号 nCmd：命令号 data：二进制数据
*/
void MainWindow::OnSndServerMsg(qint16 nSystem, qint16 nCmd, std::string data)
{
    // 先计算出包体的总长度
    // 因为packet类增加字符串的时候会增加2字节的长度和1字节的结束字符
    // 所以除了nSystem和nCmd之外需要多增加3字节的数据长度
    int nDataLength = sizeof(nSystem) + sizeof(nCmd) + 3 + data.length();
    Packet packet;
    packet << nDataLength << nSystem << nCmd << data.c_str();

    m_ServerSockect->write(packet.getDataBegin(), packet.getLength());
}

void MainWindow::OnServerMsgRecv()
{
    //先解析数据头，整个数据包包含多少个字节
    char headStr[4];
    if(m_ServerSockect->read(headStr, 4) == 0)
    {
        qDebug() << "服务器请求断开连接";
        return;
    }
    int packetLength = *(int*)headStr;
    if (packetLength <= 0 && packetLength >= 65536)
    {
        qDebug() << "数据包头读取出错：" << packetLength;
        m_ServerSockect->readAll();
        return;
    }
    //解析数据部分
    char dataStr[packetLength];
    qint16 readCount = m_ServerSockect->read(dataStr, packetLength);
    if (readCount != packetLength)
    {
        qDebug() << "数据包解析错误，丢弃当前数据包";
        m_ServerSockect->readAll();
        return;
    }
    Packet packet(dataStr, packetLength);
    qint16 nSystem, nCmd;
    packet >> nSystem >> nCmd;

    qDebug() << "nSystem = " << nSystem << ", nCmd = " << nCmd;

}

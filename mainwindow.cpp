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

    init_windows();
}

MainWindow::~MainWindow()
{
    m_ServerSockect->close();
    delete ui;
}

void MainWindow::SetLoginDialog(LoginDialog* dialog)
{
    m_loginDailog = dialog;
}

void MainWindow::init_windows()
{
    m_mainWindowWidget = new QWidget(this);
    m_leftWidget = new QWidget(this);
    m_rightWidget = new QWidget(this);

    vlayout_left = new QVBoxLayout;
    vlayout_right = new QVBoxLayout;
    hlayout_all = new QHBoxLayout;

    //左边
    m_treeWidget = new QTreeWidget(this);
    vlayout_left->addWidget(m_treeWidget);

    //右边
    m_tabWidget = new QTabWidget(this);
    m_lineEdit = new QLineEdit(this);

    vlayout_right->addWidget(m_lineEdit);
    vlayout_right->addWidget(m_tabWidget);

    vlayout_right->setStretchFactor(m_lineEdit, 2);
    vlayout_right->setStretchFactor(m_tabWidget, 10);

    m_leftWidget->setLayout(vlayout_left);
    m_rightWidget->setLayout(vlayout_right);

    hlayout_all->addWidget(m_leftWidget);
    hlayout_all->addWidget(m_rightWidget);
    hlayout_all->setStretchFactor(m_leftWidget, 1);
    hlayout_all->setStretchFactor(m_rightWidget, 5);

    m_mainWindowWidget->setLayout(hlayout_all);

    setCentralWidget(m_mainWindowWidget);

    QWidget* widget = new QWidget(this);
    QWidget* widget1 = new QWidget(this);
    QWidget* widget2 = new QWidget(this);
    m_tabWidget->addTab(widget, "111");
    m_tabWidget->addTab(widget1, "222");
    m_tabWidget->addTab(widget2, "333");
}

//窗口关闭事件
void MainWindow::closeEvent(QCloseEvent *)
{
    //如果不加这行的处理，会因为有一个隐藏的loginDialog而卡住
    QApplication::setQuitOnLastWindowClosed(true);
}

//请求连接到特定服务器
void MainWindow::OnClickConnectServerBtn(QString ip, qint32 port)
{
    qDebug() << "请求连接服务器: " << ip << ", " << port;
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
}

//服务器连接成功
void MainWindow::OnServerConnect()
{
    this->show();
    m_loginDailog->hide();
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

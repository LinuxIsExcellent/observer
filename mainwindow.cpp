#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "parseconfig.h"
#include "globalconfig.h"
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include "log.h"

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

    connect(m_treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(OnClickTreeWidgetItem(QTreeWidgetItem *, int)));
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
    hlayout_all->setStretchFactor(m_rightWidget, 4);

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

//双击文件树的item
void MainWindow::OnClickTreeWidgetItem(QTreeWidgetItem *item, int column)
{
    if(item && item->parent())
    {
        if (item->parent()->text(0) == tr("二维表"))
        {
            qDebug() << item->text(0);
            //请求二维表的数据

            test_2::client_lua_table_data_quest quest;
            quest.set_file_name(item->text(0).toStdString());

            std::string output;
            quest.SerializeToString(&output);

            OnSndServerMsg(0, test_2::client_msg::REQUSET_LUA_TABLE_INFO, output);
        }
    }
}

//服务器连接成功
void MainWindow::OnServerConnect()
{
    this->show();
    m_loginDailog->hide();

    test_2::client_login_request quest;
    std::string output;
    quest.SerializeToString(&output);

    OnSndServerMsg(0, test_2::client_msg::REQUEST_LOGIN, output);
}

/*发送数据包
 * in nSystem：系统号 nCmd：命令号 data：二进制数据
*/
void MainWindow::OnSndServerMsg(quint16 nSystem, quint16 nCmd, std::string data)
{
    // 先计算出包体的总长度
    // 因为packet类增加字符串的时候会增加2字节的长度和1字节的结束字符
    // 所以除了nSystem和nCmd之外需要多增加3字节的数据长度
    quint32 nDataLength = sizeof(nSystem) + sizeof(nCmd) + 3 + data.length();
    Packet packet;
    packet << nDataLength << nSystem << nCmd << data.c_str();

    m_ServerSockect->write(packet.getDataBegin(), packet.getLength());
}

void MainWindow::OnServerMsgRecv()
{
    //把内核缓冲区中所有的字节流读取出来
    QByteArray data = m_ServerSockect->readAll();

    quint32 nBufferSize = m_RecvBuffer.size();
    if (nBufferSize <= RECV_BUFFER_SIZE)
    {
        m_RecvBuffer.append(data);
    }
    else
    {
        qDebug() << "tcp数据解析错误，数据缓存区已满 :" << nBufferSize;
        m_RecvBuffer.clear();
    }

    nBufferSize = m_RecvBuffer.size();

    QByteArray header = m_RecvBuffer.left(4);
    quint32 nLength = *(quint32*)header.data();

    qDebug() << "数据包的大小为: " << nLength;
    qDebug() << "接收到的流的大小为: " << data.size();
    qDebug() << "数据缓冲区的大小: " << nBufferSize;

    if (nBufferSize - 4 >= nLength)
    {
        char* packetStr = m_RecvBuffer.data();
        Packet packet(packetStr + 4, nLength);
        OnNetMsgProcess(packet);

        //重新设置接收缓冲区的数据
        if (nBufferSize - 4 > nLength)
        {
            QByteArray rightData = m_RecvBuffer.right(nBufferSize - nLength - 4);
            m_RecvBuffer.clear();
            m_RecvBuffer = rightData;

            qDebug() << "重置缓冲区";
        }
        else
        {
            m_RecvBuffer.clear();

            qDebug() << "重置缓冲区";
        }
    }
//    qDebug() << "head = " << data/*.size()*/;

//    //先解析数据头，整个数据包包含多少个字节
//    char headStr[4];
//    if(m_ServerSockect->read(headStr, 4) == 0)
//    {
//        qDebug() << "服务器请求断开连接";
//        return;
//    }

//    int nMax = 0x3FFFFFF;
//    qDebug() << "nMax = " << nMax;

//    int packetLength = *(int*)headStr;
//    if (packetLength <= 0 || packetLength >= 65536)
//    {
//        qDebug() << "数据包头读取出错：" << packetLength;
//        m_ServerSockect->readAll();
//        return;
//    }
//    //解析数据部分
//    char dataStr[packetLength];
//    quint16 readCount = m_ServerSockect->read(dataStr, packetLength);
//    qDebug() << "readCount = " << readCount;
//    qDebug() << "packetLength = " << packetLength;
//    if (readCount != packetLength)
//    {
//        qDebug() << "数据包解析错误，丢弃当前数据包";
//        QByteArray data = m_ServerSockect->readAll();
//        qDebug() << "size = " << data.size();
//        return;
//    }
//    Packet packet(dataStr, packetLength);
//    OnNetMsgProcess(packet);
}

void MainWindow::OnNetMsgProcess(Packet& packet)
{
    quint16 nSystem, nCmd;
    const char* strData;

    packet >> nSystem >> nCmd >> strData;
    qDebug() << "nSystem = " << nSystem << ", nCmd = " << nCmd;

    if (nSystem == 0)
    {
        if (nCmd == test_2::server_msg::SEND_FILE_TREE_INFO)
        {
            test_2::server_send_file_tree_notify notify;
            notify.ParseFromString(strData);

            OnLeftTreeViewData(notify);
        }
        else if (nCmd == test_2::server_msg::SEND_LUA_TABLE_DATA)
        {
//            qDebug() << strData;
        }
    }
}

//https://blog.csdn.net/weixin_39485901/article/details/88413789
void MainWindow::OnLeftTreeViewData(test_2::server_send_file_tree_notify& proto)
{
    m_treeWidget->clear();
    m_treeWidget->setHeaderHidden(true);

    QList<QTreeWidgetItem *> items;

    QTreeWidgetItem* first = new QTreeWidgetItem;
    first->setText(0, tr("二维表"));
    items.append(first);

    QTreeWidgetItem* second = new QTreeWidgetItem;
    second->setText(0, tr("全局一维表"));
    items.append(second);

    QTreeWidgetItem* third = new QTreeWidgetItem;
    third->setText(0, tr("组合"));
    items.append(third);

    m_treeWidget->insertTopLevelItems(0, items);


    for (int i = 0; i < proto.lua_table_file_names_size();++i)
    {
        std::string file_name = proto.lua_table_file_names(i);

        QTreeWidgetItem *itemFileName = new QTreeWidgetItem;
        itemFileName->setText(0, QString::fromStdString(file_name));
        first->addChild(itemFileName);
    }
}

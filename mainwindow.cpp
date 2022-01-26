#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "parseconfig.h"
#include "globalconfig.h"
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include "log.h"
#include "tabwidgetcell.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(1280, 720);

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

    m_tabWidget->setMovable(true);
    m_tabWidget->setTabsClosable(true);

    connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(OnCloseTabWidgetSlot(int)));
}

void MainWindow::OnCloseTabWidget(QWidget* widget)
{
    if(!widget) return;

    int nIndex = m_tabWidget->indexOf(widget);

    m_mTabwidgetMap.erase(m_mTabwidgetMap.find(m_tabWidget->tabText(nIndex)));
    m_tabWidget->removeTab(nIndex);

    delete widget;
    widget = nullptr;
}

void MainWindow::OnCloseTabWidgetSlot(int nIndex)
{
    TabWidgetCell* tabCell = (TabWidgetCell*)m_tabWidget->widget(nIndex);
    if(tabCell)
    {
        //如果表头顺序有变化
        if (tabCell->IsHeadIndexChange() || tabCell->IsTableDataChange())
        {
            QMessageBox box(QMessageBox::Warning,QString::fromLocal8Bit("保存修改？"),QString::fromLocal8Bit("表被修改，是否保存？"));
            QPushButton *saveButton = (box.addButton(QString::fromLocal8Bit("保存"),QMessageBox::AcceptRole));
            QPushButton *quitButton = (box.addButton(QString::fromLocal8Bit("退出"),QMessageBox::AcceptRole));
            QPushButton *cancelButton = (box.addButton(QString::fromLocal8Bit("取消"),QMessageBox::RejectRole));
            cancelButton->hide();
            box.exec();

            //请求保存再关闭界面
            if( box.clickedButton() == saveButton )
            {
                //请求保存修改 TODO
                OnCloseTabWidget(tabCell);
            }
            //直接关闭界面
            else if ( box.clickedButton() == quitButton )
            {
                OnCloseTabWidget(tabCell);
            }
            //退出message对话框（直接关闭messageBox对话框）
            else if ( box.clickedButton() == cancelButton )
            {
                return;
            }
        }
        else
        {
            OnCloseTabWidget(tabCell);
        }
    }
}

//窗口关闭事件
void MainWindow::closeEvent(QCloseEvent *)
{
    //如果不加这行的处理，会因为有一个隐藏的loginDialog而卡住
    QApplication::setQuitOnLastWindowClosed(true);
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_S  &&  ev->modifiers() == Qt::ControlModifier)
    {
       TabWidgetCell* tabCell = (TabWidgetCell*)m_tabWidget->currentWidget();
       if(tabCell)
       {
           tabCell->OnRequestSaveData();
       }
       return;
    }
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

            OnSndServerMsg(0, test_2::client_msg::REQUSET_LUA_TABLE_DATA, output);
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
            test_2::table_data notify;
            notify.ParseFromString(strData);

            OnRecvServerLuaTableData(notify);
        }
    }
}

void MainWindow::OnRecvServerLuaTableData(test_2::table_data& proto)
{
    QString table_name = QString::fromStdString(proto.table_name());
    auto iter = m_mTabwidgetMap.find(table_name);
    //如果tab里面有这个widget
    if (iter != m_mTabwidgetMap.end())
    {
        m_tabWidget->setCurrentWidget(iter.value());
        iter.value()->SetProtoData(proto);
    }
    else
    {
        TabWidgetCell* tabCell = new TabWidgetCell(m_tabWidget);
        if(tabCell)
        {
            tabCell->SetTabWidget(m_tabWidget);
            tabCell->SetManWindows(this);

            m_tabWidget->addTab(tabCell, table_name);
            tabCell->SetProtoData(proto);

            m_mTabwidgetMap.insert(table_name, tabCell);
            m_tabWidget->setCurrentWidget(tabCell);
        }

//        QWidget* widget = new QWidget(this);
//        m_tabWidget->addTab(widget, table_name);
//        m_mTabwidgetMap.insert(table_name, widget);
//        m_tabWidget->setCurrentWidget(widget);


//        QTableView* tableView = new QTableView(widget);
//        if(tableView)
//        {
//            tableView->verticalHeader()->hide();
//            tableView->horizontalHeader()->setSectionsMovable(true);

//            QHBoxLayout* layout = new QHBoxLayout(widget);
//            layout->addWidget(tableView);
//            widget->setLayout(layout);

//            QStandardItemModel *student_model = new QStandardItemModel();
//            student_model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("ID")));
//            //利用setModel()方法将数据模型与QTableView绑定
//            tableView->setModel(student_model);

//            int nRow = proto.row_count();
//            int nColumn = proto.column_count();

//            for (int i = 0; i < proto.row_lists_size();++i)
//            {
//                test_2::row_data row_data = proto.row_lists(i);
//                student_model->setItem(i, 0, new QStandardItem(QString::fromStdString(row_data.key())));

//                for (int j = 0; j < row_data.pair_size(); ++j) {
//                    test_2::pair_value pair = row_data.pair(j);

//                    student_model->setHorizontalHeaderItem(j + 1, new QStandardItem(QString::fromStdString(pair.key())));
//                    student_model->setItem(i, j + 1, new QStandardItem(QString::fromStdString(pair.value())));
//                }
//            }

//            tableView->resizeColumnsToContents();
//        }
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

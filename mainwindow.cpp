#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "parseconfig.h"
#include "globalconfig.h"
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include "log.h"
#include "luatabledatawidget.h"
#include "lualistdatawidget.h"
#include "qloadingwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(1280, 720);

    m_menu_bar = new QMenuBar(this);             //创建一个菜单栏
    this->setMenuBar(m_menu_bar);

    QMenu *file_menu = new QMenu("文件(&F)",m_menu_bar);
    m_menu_bar->addMenu(file_menu);

    m_dShellScriptOpPrintDlg = new ShowMsgDialog(this);

    m_ServerSockect = new QTcpSocket(this);
    //连接事件
    connect(m_ServerSockect,&QTcpSocket::connected, this, &MainWindow::OnServerConnect);
    //有可读事件
    connect(m_ServerSockect,&QTcpSocket::readyRead, this, &MainWindow::OnServerMsgRecv);
    //socket错误事件
    connect(m_ServerSockect, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnSocketError(QAbstractSocket::SocketError)));

    init_windows();

    //设置去掉右下角的三角
    ui->statusbar->setSizeGripEnabled(false);

    connect(m_treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(OnClickTreeWidgetItem(QTreeWidgetItem *, int)));

    m_loadingDialog = new QLoadingWidget(this);
    m_loadingDialog->hide();
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

    m_timeLabel=new QLabel(this);
    m_timeLabel->setFrameStyle(QFrame::Box|QFrame::Sunken);
    m_timeLabel->setToolTip(tr("双击修改服务器时间"));
    ui->statusbar->addPermanentWidget(m_timeLabel);//显示永久信息

    m_timeLabel->installEventFilter(this);

    m_1sTimer = new QTimer(this);
    connect(m_1sTimer, SIGNAL(timeout()), this, SLOT(On1STimerUpdate()));

    m_1sTimer->start(1000);

    m_timeWidget = new ModifyServerTimeWidget(this);
    connect(m_timeWidget, SIGNAL(OnClickConfirmBtn(quint64)), this, SLOT(OnRequestModifyServerTime(quint64)));
    m_timeWidget->hide();

    m_addFieldLinkDialog = new AddFieldLinkDialog(this);
}

void MainWindow::OnOpenAddLinkFieldDialog(TabWidgetCell* widget, QString sField, bool rootWidget/* = true*/)
{
    if (m_addFieldLinkDialog)
    {
        m_addFieldLinkDialog->OnShow(widget, sField, rootWidget);

        test_2::client_field_link_info_quest quest;
        std::string output;
        quest.SerializeToString(&output);

        OnSndServerMsg(0, test_2::client_msg::REQUSET_FIELD_LINK_INFO, output);
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick)
    {
        qDebug() << "click";
    }
    if (qobject_cast<QLabel*>(watched) == m_timeLabel && event->type() == QEvent::MouseButtonDblClick)
    {
        m_timeWidget->setGeometry(m_timeLabel->x() - m_timeWidget->width() + m_timeLabel->width(),
                                  ui->statusbar->y() - m_timeWidget->height(), m_timeWidget->width(), m_timeWidget->height());
        m_timeWidget->SetTime(m_serverTimeStamp);
        m_timeWidget->show();
    }
    return false;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    m_timeWidget->setGeometry(m_timeLabel->x() - m_timeWidget->width() + m_timeLabel->width(),
                              ui->statusbar->y() - m_timeWidget->height(), m_timeWidget->width(), m_timeWidget->height());
}

void MainWindow::On1STimerUpdate()
{
    m_serverTimeStamp += 1;
    QDateTime time = QDateTime::fromTime_t(m_serverTimeStamp);
    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");

    m_timeLabel->setText(tr("服务器时间：") + strTime);

    if (!m_timeWidget->isHidden())
    {
        m_timeWidget->TimeTick();
    }
}

void MainWindow::OnMenuActionTriggered()
{
    QAction* qAction = qobject_cast<QAction *>(sender());
    if (qAction)
    {
        test_2::client_shell_option_quest quest;
        quest.set_option(qAction->text().toStdString());

        std::string output;
        quest.SerializeToString(&output);

        OnSndServerMsg(0, test_2::client_msg::REQUSET_SHELL_OPTIONS, output);
    }
}

void MainWindow::OnCloseTabWidget(QWidget* widget)
{
    if(!widget) return;

    TabWidgetCell* tabCell = (TabWidgetCell*)widget;
    if (tabCell)
    {
        QString sTableName = m_mTabwidgetMap.key(tabCell);

        m_mTabwidgetMap.erase(m_mTabwidgetMap.find(sTableName));
    }

    int nIndex = m_tabWidget->indexOf(widget);
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
        if (tabCell->IsTableDataChange())
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
    if (ev->modifiers() == Qt::ControlModifier)
    {
       TabWidgetCell* tabCell = (TabWidgetCell*)m_tabWidget->currentWidget();
       if (ev->key() == Qt::Key_S)
       {
           if(tabCell)
           {
               tabCell->OnRequestSaveData();
               return;
           }
       }
       else if (ev->key() == Qt::Key_W) {
            OnCloseTabWidgetSlot(m_tabWidget->currentIndex());
       }
       else if (ev->key() == Qt::Key_Z)
       {
           if(tabCell)
           {
               tabCell->undo();
               return;
           }
       }
       else if (ev->key() == Qt::Key_Y)
       {
           if(tabCell)
           {
               tabCell->undo();
               return;
           }
       }
    }
    else if (ev->key() == Qt::Key_Escape)
    {
        TabWidgetCell* tabCell = (TabWidgetCell*)m_tabWidget->currentWidget();
        if(tabCell)
        {
            tabCell->GlobalKeyPressEevent(ev);
            return;
        }
    }
}

void MainWindow::OnRequestModifyServerTime(quint64 nTime)
{
    qDebug() << "asdsa = " << nTime;
    test_2::client_modify_server_time_quest quest;
    quest.set_time(nTime);

    std::string output;
    quest.SerializeToString(&output);

    OnSndServerMsg(0, test_2::client_msg::REQUEST_MODIFY_SERVER_TIME, output);
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
void MainWindow::OnClickTreeWidgetItem(QTreeWidgetItem *item, int)
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

//            setCursor(Qt::WaitCursor);
//            m_loadingDialog->show();

            OnSndServerMsg(0, test_2::client_msg::REQUSET_LUA_TABLE_DATA, output);
        }
        else if (item->parent()->text(0) == tr("全局一维表"))
        {
            test_2::client_lua_table_data_quest quest;
            quest.set_file_name(item->text(0).toStdString());

            std::string output;
            quest.SerializeToString(&output);

            OnSndServerMsg(0, test_2::client_msg::REQUSET_LUA_LIST_DATA, output);
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

    qDebug() << "请求服务器消息:" << nSystem << "  " << nCmd;
    int nSendCount = m_ServerSockect->write(packet.getDataBegin(), packet.getLength());
    qDebug() << "nSendCount = " << nSendCount;
    m_ServerSockect->flush();
}

void MainWindow::OnServerMsgRecv()
{
    //把内核缓冲区中所有的字节流读取出来
    QByteArray data = m_ServerSockect->readAll();
    if (data.count() <= 0) return;


    qDebug() << "还有多少字节可以读取：" << m_ServerSockect->bytesAvailable();
    quint32 nBufferSize = m_RecvBuffer.size();
    qDebug() << "nBufferSize = " << nBufferSize;
    if (nBufferSize <= RECV_BUFFER_SIZE)
    {
        qDebug() << "new datasize : " << data.size();
        m_RecvBuffer.append(data);

        qDebug() << "after buffersize : " << m_RecvBuffer.size();
    }
    else
    {
//        qDebug() << "tcp数据解析错误，数据缓存区已满 :" << nBufferSize;
        m_RecvBuffer.clear();
    }

    bool bProcessLoop = true;
    while(bProcessLoop)
    {
        nBufferSize = m_RecvBuffer.size();

        QByteArray header = m_RecvBuffer.left(4);
        quint32 nLength = *(quint32*)header.data();

        qDebug() << "nLength = " << nLength << ", nBufferSize = " << nBufferSize;
        if (nBufferSize - 4 >= nLength)
        {
            char* packetStr = m_RecvBuffer.data();
            Packet packet(packetStr + 4, nLength);
            OnNetMsgProcess(packet);

            //重新设置接收缓冲区的数据
            if (nBufferSize - 4 > nLength)
            {
                QByteArray rightData = m_RecvBuffer.right(nBufferSize - nLength - 4);
                m_RecvBuffer = rightData;
//                m_RecvBuffer.clear();
            }
            else
            {
                m_RecvBuffer.clear();
                qDebug() << "clear after = " << m_RecvBuffer.size();
                bProcessLoop = false;
            }
        }
        else
        {
            bProcessLoop = false;
        }
    }
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
        else if (nCmd == test_2::server_msg::SEND_SERVER_TIME)
        {
            test_2::send_server_current_time_nofity notify;
            notify.ParseFromString(strData);

            OnRecvServerSendCurrentTime(notify);
        }
        else if (nCmd == test_2::server_msg::SEND_LUA_TABLE_DATA)
        {
            test_2::table_data notify;
            notify.ParseFromString(strData);

            OnRecvServerLuaTableData(notify);
        }
        else if (nCmd == test_2::server_msg::SEND_SHELL_CONFIG)
        {
            test_2::server_send_shell_config_notify notify;
            notify.ParseFromString(strData);

            OnRecvServerShellOpsData(notify);
        }
        else if (nCmd == test_2::server_msg::SEND_OPTION_SHELL_PRINT)
        {
            test_2::send_shell_option_print_notify notify;
            notify.ParseFromString(strData);

            OnRecvServerShellOptionPrint(notify);
        }
        else if (nCmd == test_2::server_msg::SEND_LUA_LIST_DATA)
        {
            test_2::send_lua_list_data_notify notify;
            notify.ParseFromString(strData);

            OnRecvServerLuaListData(notify);
        }
        else if (nCmd == test_2::server_msg::SEND_FIELD_LINK_DATA)
        {
            test_2::send_field_link_info notify;
            notify.ParseFromString(strData);

            OnRecvServerFieldLinkInfo(notify);
        }
        else if (nCmd == test_2::server_msg::SEND_PROCESS_STATUS_INFO)
        {
            test_2::send_process_listening_status_info notify;
            notify.ParseFromString(strData);


            OnRecvServerProcessStatusInfo(notify);
        }
    }
}

void MainWindow::OnRecvServerProcessStatusInfo(const test_2::send_process_listening_status_info& proto)
{
    for (int i = 0;i < proto.infos_size();++i)
    {
        test_2::process_statue_info info = proto.infos(i);

        QString sStatus = QString::fromStdString(info.process_name());
        QString sStyleSheet = "color:red;";

        if (info.statue() == 0)
        {
            sStatus = sStatus + " (关闭)";
        }
        else
        {
            sStyleSheet = "color:green;";
            sStatus = sStatus + " (运行)";
        }

        if (i > m_vProcessStatusLabList.size() - 1)
        {
            QLabel* Label =new QLabel(this);
            Label->setFrameStyle(QFrame::Box|QFrame::Sunken);
            Label->setText(sStatus);
            Label->setStyleSheet(sStyleSheet);
            ui->statusbar->insertWidget(0, Label);

            m_vProcessStatusLabList.push_back(Label);
        }
        else
        {
            m_vProcessStatusLabList[i]->setText(sStatus);
            m_vProcessStatusLabList[i]->setStyleSheet(sStyleSheet);
        }
    }
}

void MainWindow::OnRecvServerFieldLinkInfo(const test_2::send_field_link_info& notify)
{
    if (m_addFieldLinkDialog)
    {
        m_addFieldLinkDialog->OnSetProtoFieldLinkInfo(notify);
    }
}

void MainWindow::OnRecvServerSendCurrentTime(const test_2::send_server_current_time_nofity& proto)
{
    quint64 nTime = proto.time();

    m_serverTimeStamp = nTime;
    On1STimerUpdate();
}

void MainWindow::OnRecvServerShellOptionPrint(const test_2::send_shell_option_print_notify& proto)
{
    //执行结束
    if (proto.flag() == 0)
    {
        if (m_dShellScriptOpPrintDlg->isHidden())
        {
            m_dShellScriptOpPrintDlg->setModal(true);
            m_dShellScriptOpPrintDlg->show();
            m_dShellScriptOpPrintDlg->SetCloseBtnDisable(true);
            m_dShellScriptOpPrintDlg->ClearTextEditLine();
        }
    }
    else if(proto.flag() == 1)
    {
        m_dShellScriptOpPrintDlg->SetCloseBtnDisable(false);
    }

    m_dShellScriptOpPrintDlg->AppendMsg(QString::fromStdString(proto.line()));
}

void MainWindow::OnRecvServerShellOpsData(const test_2::server_send_shell_config_notify& proto)
{
    QMenu *edit_menu = new QMenu("脚本(&S)",m_menu_bar);
    for (int i = 0; i < proto.shell_ops_size();++i)
    {
        std::string file_name = proto.shell_ops(i);

        QAction *new_action = new QAction(QString::fromStdString(file_name));
        new_action->trigger();
        edit_menu->addAction(new_action);

        connect(new_action, SIGNAL(triggered()), this, SLOT(OnMenuActionTriggered()));
    }

    m_menu_bar->addMenu(edit_menu);
}

void MainWindow::OnRecvServerLuaListData(const test_2::send_lua_list_data_notify& proto)
{
    qDebug() << QString::fromStdString(proto.table_name());

    QString table_name = QString::fromStdString(proto.table_name());
    auto iter = m_mTabwidgetMap.find(table_name);
    //如果tab里面有这个widget
    if (iter != m_mTabwidgetMap.end())
    {
        m_tabWidget->setCurrentWidget(iter.value());
        qobject_cast<LuaListDataWidget*>(iter.value())->SetProtoData(proto);
    }
    else
    {
        LuaListDataWidget* tabCell = new LuaListDataWidget(m_tabWidget);
        if(tabCell)
        {
            tabCell->SetTabWidget(m_tabWidget);
            tabCell->SetManWindows(this);

            m_tabWidget->addTab(tabCell, table_name);
            tabCell->SetName(table_name);
            tabCell->SetProtoData(proto);

            m_mTabwidgetMap.insert(table_name, tabCell);
            m_tabWidget->setCurrentWidget(tabCell);
        }
    }
}

void MainWindow::OnRecvServerLuaTableData(const test_2::table_data& proto)
{
    QString table_name = QString::fromStdString(proto.table_name());
    auto iter = m_mTabwidgetMap.find(table_name);
    //如果tab里面有这个widget
    if (iter != m_mTabwidgetMap.end())
    {
        m_tabWidget->setCurrentWidget(iter.value());
        qDebug() << "重设数据 : " << table_name;
        qobject_cast<LuaTableDataWidget*>(iter.value())->SetProtoData(proto);
    }
    else
    {
        LuaTableDataWidget* tabCell = new LuaTableDataWidget(m_tabWidget);
        if(tabCell)
        {
            tabCell->SetTabWidget(m_tabWidget);
            tabCell->SetManWindows(this);

            m_tabWidget->addTab(tabCell, table_name);
            tabCell->SetName(table_name);
            tabCell->SetProtoData(proto);

            m_mTabwidgetMap.insert(table_name, tabCell);
            m_tabWidget->setCurrentWidget(tabCell);
        }
    }
}

//https://blog.csdn.net/weixin_39485901/article/details/88413789
void MainWindow::OnLeftTreeViewData(const test_2::server_send_file_tree_notify& proto)
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

    for (int i = 0; i < proto.lua_file_names_size();++i)
    {
        std::string file_name = proto.lua_file_names(i);

        QTreeWidgetItem *itemFileName = new QTreeWidgetItem;
        itemFileName->setText(0, QString::fromStdString(file_name));
        second->addChild(itemFileName);
    }
}

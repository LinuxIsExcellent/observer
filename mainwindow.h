#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>
#include <QLineEdit>
#include <QTreeWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QTableView>
#include <QStandardItemModel>
#include <QLabel>
#include <QDateTimeEdit>
#include <QTimer>
#include "logindialog.h"
#include "showmsgdialog.h"
#include "Packet.h"
#include "msg.pb.h"
#include "modifyservertimewidget.h"
#include "addfieldlinkdialog.h"
#include <google/protobuf/text_format.h>

#define RECV_BUFFER_SIZE 20 * 1024 * 1024

class LoginDialog;
class ShowMsgDialog;
class TabWidgetCell;
class LuaTableDataWidget;
class LuaListDataWidget;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init_windows();

    void OnClickConnectServerBtn(QString ip, qint32 port);

    void SetLoginDialog(LoginDialog* dialog);

    void OnNetMsgProcess(Packet& packet);

    void OnCloseTabWidget(QWidget* widget);

    void OnOpenAddLinkFieldDialog(QWidget* widget, bool rootWidget = true, quint16 nIndex = 0);
public:
    //刷新左边的列表数据
    void OnLeftTreeViewData(const test_2::server_send_file_tree_notify& proto);

    //收到服务器发送过来的二维表数据，刷新tableWidget
    void OnRecvServerLuaTableData(const test_2::table_data& proto);

    //收到服务器发过来的shell操作指令列表
    void OnRecvServerShellOpsData(const test_2::server_send_shell_config_notify& proto);

    //收到服务器发过来的shell指令执行的打印
    void OnRecvServerShellOptionPrint(const test_2::send_shell_option_print_notify& proto);

    //收到服务器推送的服务器时间
    void OnRecvServerSendCurrentTime(const test_2::send_server_current_time_nofity& proto);

    //收到服务器发来的一维表数据
    void OnRecvServerLuaListData(const test_2::send_lua_list_data_notify& proto);

    //收到服务器发来的关联信息
    void OnRecvServerFieldLinkInfo(const test_2::send_field_link_info& proto);
protected:
    void closeEvent(QCloseEvent *event);

    virtual void keyPressEvent(QKeyEvent *ev);

    bool eventFilter(QObject *watched, QEvent *event);

    void resizeEvent(QResizeEvent *event);
public slots:

    void OnServerConnect();

    void OnSocketError(QAbstractSocket::SocketError error);

    void OnServerMsgRecv();

    void OnSndServerMsg(quint16 nSystem, quint16 nCmd, std::string data);

    void OnClickTreeWidgetItem(QTreeWidgetItem *item, int);

    void OnCloseTabWidgetSlot(int nIndex);

    void OnMenuActionTriggered();

    void On1STimerUpdate();

    //请求修改服务器时间
    void OnRequestModifyServerTime(quint64 nTime);
private:
    Ui::MainWindow *ui;

    QByteArray m_RecvBuffer;      //tcp数据缓冲区

    QTcpSocket* m_ServerSockect;
    LoginDialog* m_loginDailog;

    QMenuBar *m_menu_bar;       //菜单栏
    ShowMsgDialog*   m_dShellScriptOpPrintDlg;    //展示shell执行结果的对话框

    //mainwindows中的控件
    QWidget* m_mainWindowWidget;  //主要的widget

    QWidget*    m_leftWidget;   //左边的widget
    QVBoxLayout* vlayout_left;

    QWidget*    m_rightWidget;  //右边的widget
    QVBoxLayout* vlayout_right;

    QTreeWidget* m_treeWidget;   //文件目录
    QTabWidget* m_tabWidget;    //数据信息tab控件
    QMap<QString, TabWidgetCell*> m_mTabwidgetMap;  //数据信息展示widget

    QLineEdit*  m_lineEdit;     //搜索栏

    QHBoxLayout* hlayout_all;        //中心widget的布局

    quint64     m_serverTimeStamp;  //服务器时间戳
    QTimer*     m_1sTimer;      //每秒定时器
    QLabel*     m_timeLabel;      //显示时间的标签

    ModifyServerTimeWidget* m_timeWidget; //修改服务器时间widget
    AddFieldLinkDialog*     m_addFieldLinkDialog; //增加关联的界面
};
#endif // MAINWINDOW_H

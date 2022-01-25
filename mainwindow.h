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
#include "logindialog.h"
#include "tabwidgetcell.h"
#include "Packet.h"
#include "msg.pb.h"
#include <google/protobuf/text_format.h>

#define RECV_BUFFER_SIZE 10 * 1024 * 1024

class LoginDialog;
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
public:
    //刷新左边的列表数据
    void OnLeftTreeViewData(test_2::server_send_file_tree_notify& proto);

    //收到服务器发送过来的二维表数据，刷新tableWidget
    void OnRecvServerLuaTableData(test_2::table_data& proto);
protected:
    void closeEvent(QCloseEvent *event);
public slots:

    void OnServerConnect();

    void OnSocketError(QAbstractSocket::SocketError error);

    void OnServerMsgRecv();

    void OnSndServerMsg(quint16 nSystem, quint16 nCmd, std::string data);

    void OnClickTreeWidgetItem(QTreeWidgetItem *item, int column);

    void OnCloseTabWidgetSlot(int nIndex);
private:
    Ui::MainWindow *ui;

    QByteArray m_RecvBuffer;      //tcp数据缓冲区

    QTcpSocket* m_ServerSockect;
    LoginDialog* m_loginDailog;

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
};
#endif // MAINWINDOW_H

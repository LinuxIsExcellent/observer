#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>
#include<QLineEdit>
#include<QTreeWidget>
#include<QTabWidget>
#include<QVBoxLayout>
#include "logindialog.h"
#include "Packet.h"

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
protected:
    void closeEvent(QCloseEvent *event);
public slots:

    void OnServerConnect();

    void OnSocketError(QAbstractSocket::SocketError error);

    void OnServerMsgRecv();

    void OnSndServerMsg(quint16 nSystem, quint16 nCmd, std::string data);
private:
    Ui::MainWindow *ui;

    QTcpSocket* m_ServerSockect;
    LoginDialog* m_loginDailog;

    //mainwindows中的控件
    QWidget* m_mainWindowWidget;  //主要的widget

    QWidget*    m_leftWidget;   //左边的widget
    QVBoxLayout* vlayout_left;

    QWidget*    m_rightWidget;  //右边的widget
    QVBoxLayout* vlayout_right;

    QTreeWidget* m_treeWidget;   //文件目录
    QTabWidget* m_tabWidget;    //数据信息展示页
    QLineEdit*  m_lineEdit;     //搜索栏

    QHBoxLayout* hlayout_all;        //中心widget的布局
};
#endif // MAINWINDOW_H

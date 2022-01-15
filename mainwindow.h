#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>

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
public slots:
    void OnClickConnectServerBtn();

    void OnServerConnect();

    void OnSocketError(QAbstractSocket::SocketError error);

    void OnServerMsgRecv();

    void OnShowProgramServerLists(const QString & str = "");
private:
    Ui::MainWindow *ui;

    QTcpSocket* m_ServerSockect;
};
#endif // MAINWINDOW_H

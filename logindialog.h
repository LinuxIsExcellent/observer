#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>
#include "mainwindow.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

private slots:
    void OnShowProgramServerLists(const QString & str = "");

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    void SetMainWindow(MainWindow* mainWindow)
    {
        m_mainWindow = mainWindow;
    }

    void InitDialog();

    void OnShowError(QMessageBox::Icon icon, QString str);
protected:
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

public slots:
    //请求连接服务器
    void OnConnectBtnClicked();

    //请求退出
    void OnclickCloseBtn();

    //点击ListView
    void OnListViewDoubleClicked(const QModelIndex &);
private:
    Ui::LoginDialog *ui;

    sProgram m_sProgram;

    MainWindow* m_mainWindow;

    bool m_bPressed;
    QPoint m_ptPress;
};

#endif // LOGINDIALOG_H

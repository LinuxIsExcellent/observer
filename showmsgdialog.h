#ifndef SHOWMSGDIALOG_H
#define SHOWMSGDIALOG_H

#include <QDialog>
#include <QDebug>
#include "mainwindow.h"

namespace Ui {
class ShowMsgDialog;
}

class ShowMsgDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShowMsgDialog(QWidget *parent = nullptr);
    ~ShowMsgDialog();

    //使得关闭按钮失效
    void SetCloseBtnDisable(bool disable);

    //清空所有的消息
    void ClearTextEditLine();
    //增加信息
    void AppendMsg(const QString& str);
public slots:
    //请求退出
    void OnclickCloseBtn();

protected:
//    void mousePressEvent(QMouseEvent*);
//    void mouseMoveEvent(QMouseEvent*);
//    void mouseReleaseEvent(QMouseEvent*);

private:
    Ui::ShowMsgDialog *ui;

    bool m_bPressed;
    QPoint m_ptPress;
};

#endif // SHOWMSGDIALOG_H

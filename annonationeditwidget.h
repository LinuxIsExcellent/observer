#ifndef ANNONATIONEDITWIDGET_H
#define ANNONATIONEDITWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QKeyEvent>
#include <windows.h>        //注意头文件
#include <windowsx.h>

namespace Ui {
class AnnonationEditWidget;
}

class AnnonationEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnnonationEditWidget(QWidget *parent = nullptr);
    ~AnnonationEditWidget();

    void SetText(QString& str);
    QString GetText();

    void OnQuit();

    void OnShow(quint32 x, quint32 y, quint32 nIndex, QString str = "");

signals:
    //保存注释
    void SaveAnnonationsSignal(QString str, quint32 nIndex);
protected:

private:
    Ui::AnnonationEditWidget *ui;

    quint32     m_nIndex;
};

#endif // ANNONATIONEDITWIDGET_H

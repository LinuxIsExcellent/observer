#ifndef ANNONATIONEDITWIDGET_H
#define ANNONATIONEDITWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QKeyEvent>
#include <windows.h>        //注意头文件
#include <windowsx.h>
#include "tabwidgetcell.h"

namespace Ui {
class AnnonationEditWidget;
}

class TabWidgetCell;

class AnnonationEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnnonationEditWidget(QString sField, QString str = "", QWidget *parent = nullptr);
    ~AnnonationEditWidget();

    void SetText(QString& str);
    QString GetText();
signals:
    //保存注释
    void SaveAnnonationsSignal(QString sIndex, QString str, QString sField);
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::AnnonationEditWidget *ui;

    TabWidgetCell* tabWidget;

    QString     m_sField;
    bool        m_bModify;
};

#endif // ANNONATIONEDITWIDGET_H

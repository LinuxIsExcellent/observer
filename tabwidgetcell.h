#ifndef TABWIDGETCELL_H
#define TABWIDGETCELL_H

#include <QWidget>
#include <QHBoxLayout>
#include <QTableView>
#include <QListWidget>
#include <QStandardItemModel>
#include <QPushButton>
#include <QDebug>
#include <QKeyEvent>
#include <QMenu>
#include "mainwindow.h"
#include "annonationeditwidget.h"

#include <google/protobuf/text_format.h>

//红色的是删除的行
//黄色的是修改的单元格
//绿色的是增加的行

//调整过位置的列的列头就是黄色


namespace Ui {
class TabWidgetCell;
}

//enum RowState
//{
//    NONE = 0,
//    MODIFY = 1,
//    ADD = 2,
//    DELETE = 3,
//};

class TabWidgetCell : public QWidget
{
    Q_OBJECT

public:
    TabWidgetCell(QWidget *parent = nullptr);
    virtual ~TabWidgetCell();

    void SetName(QString sName)
    {
        m_sName = sName;
    }

    QString GetName()
    {
        return m_sName;
    }

    //把tabwidget对象的指针也带进来
    void SetTabWidget(QTabWidget* tabwidget)
    {
        m_tabWidget = tabwidget;
    }

    //把tabwidget对象的指针也带进来
    void SetManWindows(MainWindow* mainWindows)
    {
        m_mainWindow = mainWindows;
    }

    //请求保存数据
    virtual void OnRequestSaveData() {}

    virtual inline bool IsTableDataChange()
    {
        return m_bTableDataChange;
    }

    //设置数据有改变
    void SetDataModify(bool modify);

    //mainwindow传过来的键盘事件
    virtual void GlobalKeyPressEevent(QKeyEvent *ev){}
public slots:
    //刷新界面
    virtual void Flush() {};

protected:
    virtual void keyPressEvent(QKeyEvent *ev);

    void resizeEvent(QResizeEvent *event);
private slots:
    virtual void AddAnnotation() {};

    void OnItemDataChange(QStandardItem *item);

    virtual void OnSaveAnnonations(QString str, quint32 nIndex){}
public:
    Ui::TabWidgetCell *ui;

    QTabWidget* m_tabWidget;    //所在的tabwidget
    MainWindow* m_mainWindow;    //所在的mainWindow

    QWidget*    m_topWidget;   //上面的widget

    QTableView*   m_tableView;  //展示数据的二维表
    QListWidget*    m_rightButtonList;  //右边的操作按钮

    QStandardItemModel*  m_standardItemModel;   //view对应的model

    QHBoxLayout* hlayout_top;  //上面的水平布局

    QListWidget*    m_bottomButtonList; //底部的显示按钮列表
    QVBoxLayout* vlayout_all;  //整个TabWidget类的垂直布局

    QMenu*  m_tableCellMenu;        //二维表数据的菜单栏
    bool    m_bTableDataChange; //表的数据是否被更改

    AnnonationEditWidget* m_annonationWidget;   //批注界面

    QString m_sName;      //表的名字
};

#endif // TABWIDGETCELL_H

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
#include <QUndoStack>
#include <QUndoView>
#include <QAction>
#include <QItemSelectionModel>
#include "mainwindow.h"
#include "annonationeditwidget.h"

#include <google/protobuf/text_format.h>

//红色的是删除的行
//黄色的是修改的单元格
//绿色的是增加的行

//调整过位置的列的列头就是黄色
class QUndoStack;
class QUndoView;
class QTableView;
class QStandardItemModel;
class MainWindow;

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

    virtual void SetFieldLink(QString sField, QString sFieldLink){};

    //设置数据有改变
    void ChangeDataModify();

    //改变一个modelIndex的数据
    void ChangeModelIndexData(QModelIndex index, QString sData);

    //mainwindow传过来的键盘事件
    virtual void GlobalKeyPressEevent(QKeyEvent *ev){}

    void clearUndoStack()
    {
        undoStack->clear();
    }

    //撤销
    void undo();

    //返回撤销
    void redo();

    //复制单元格的内容
    void copy();

    //粘贴全局剪切板的内容
    void paste();
public slots:
    //刷新界面
    virtual void Flush() {};

protected:
    virtual void keyPressEvent(QKeyEvent *ev);

    void resizeEvent(QResizeEvent *event);

    bool eventFilter(QObject *obj, QEvent *event);
private slots:
    virtual void AddAnnotation() {};

    void OnItemDataChange(QStandardItem *item);

//    void onCurrentChanged(const QModelIndex& current, const QModelIndex& previous);

    virtual void OnSaveAnnonations(QString sIndex, QString str, QString sField){}

    void OnSaveButtonClicked();
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

    QPushButton* m_saveDataButton;  //保存数据的按钮

    AnnonationEditWidget* m_annonationWidget;   //批注界面

    QString m_sName;      //表的名字

    //实现撤销功能
    QAction *undoAction;
    QAction *redoAction;
    QUndoStack *undoStack;
    QUndoView *undoView;
};

#endif // TABWIDGETCELL_H

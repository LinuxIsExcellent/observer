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
#include <QMouseEvent>
#include <QMenu>
#include <QUndoStack>
#include <QUndoView>
#include <QAction>
#include <QItemSelectionModel>
#include <QScrollBar>
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

typedef struct fieldInfo
{
    QString sFieldName;      //字段名字
    QString sFieldAnnonation;      //字段的备注
    QString sFieldLink;      //字段的关联

    friend QDebug& operator << (QDebug out, const fieldInfo& info)
    {
        out << info.sFieldName << info.sFieldAnnonation << info.sFieldLink;

        return out;
    };
}FIELDINFO;

//表的字段信息
typedef struct fieldSquence
{
    QString sIndex;         //索引
    QVector<FIELDINFO> vSFieldSquences;       //对应的字段顺序

    friend QDebug& operator << (QDebug out, const fieldSquence& info)
    {
        out << info.sIndex << info.vSFieldSquences;

        return out;
    };
}FIELDSQUENCE;

enum TabWidgetType
{
    enTabWidgetTable = 1,
    enTabWidgetList = 2,
};

class TabWidgetCell : public QWidget
{
    Q_OBJECT

public:
    TabWidgetCell(QWidget *parent = nullptr);
    virtual ~TabWidgetCell();

    void SetType(TabWidgetType type)
    {
        m_type = type;
    }

    TabWidgetType GetType()
    {
        return m_type;
    }

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
    virtual bool OnRequestSaveData();

    void SetDataModify()
    {
        m_bTableDataChange = true;

        ChangeDataModify();
    }

    virtual inline bool IsTableDataChange()
    {
        return m_bTableDataChange;
    }

    virtual void SetFieldLink(QString sIndex, QString sField, QString sFieldLink){};

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

    //设置行高和列宽
    void SetRowAndColParam();

    virtual void OnSaveAnnonations(QString sIndex, QString str, QString sField){}

    virtual void CheckItemDataTypeIsCorrect(QStandardItem *item){};
public slots:
    //刷新界面
    virtual void Flush() {};

    //item改变
    virtual void OnItemDataChange(QStandardItem *item);

    //行大小改变
    virtual void OnRowResized(int logicalIndex, int oldSize, int newSize);

    //列大小改变
    virtual void OnColResized(int logicalIndex, int oldSize, int newSize);
protected:
    virtual void keyPressEvent(QKeyEvent *ev);

    void resizeEvent(QResizeEvent *event);

    bool eventFilter(QObject *obj, QEvent *event);
private slots:
    virtual void AddAnnotation() {};

//    void onCurrentChanged(const QModelIndex& current, const QModelIndex& previous);

    void OnSaveButtonClicked();

    void OnShowAllRow();
public:
    Ui::TabWidgetCell *ui;

    TabWidgetType m_type;
    QTabWidget* m_tabWidget;    //所在的tabwidget
    MainWindow* m_mainWindow;    //所在的mainWindow

    QWidget*    m_topWidget;   //上面的widget

    QTableView*   m_tableView;  //展示数据的二维表
    QListWidget*    m_rightButtonList;  //右边的操作按钮

    QStandardItemModel*  m_standardItemModel;   //view对应的model

    QHBoxLayout* hlayout_top;  //上面的水平布局

    QListWidget*    m_bottomButtonList; //底部的显示按钮列表
    QVBoxLayout* vlayout_all;  //整个TabWidget类的垂直布局

    QMap<QString, FIELDSQUENCE>   m_mFieldSquence;   //表的外围信息

    QMenu*  m_tableCellMenu;        //二维表数据的菜单栏

    bool    m_bTableDataChange; //表的数据是否被更改

    QPushButton* m_saveDataButton;  //保存数据的按钮    

    QString m_sName;      //表的名字

    //实现撤销功能
    QAction *undoAction;
    QAction *redoAction;
    QUndoStack *undoStack;
    QUndoView *undoView;

    QVector<int>    m_vRowHeight;       //行高
    QVector<int>    m_vColWidth;        //列宽

    int m_currentVSlider;       //记录之前的垂直滑动条位置
    int m_currentHSlider;       //记录之前的水平滑动条位置

    QMap<int, bool>             m_mTypeCheck;       //存储错误格式的类型的单元格row * 10000 + col, 是否出错
};

#endif // TABWIDGETCELL_H

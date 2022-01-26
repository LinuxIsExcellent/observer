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
#include "mainwindow.h"
#include "msg.pb.h"
#include <google/protobuf/text_format.h>

//红色的是删除的行
//黄色的是修改的单元格
//绿色的是增加的行

//调整过位置的列的列头就是黄色


namespace Ui {
class TabWidgetCell;
}

class MainWindow;

enum RowState
{
    NONE = 0,
    MODIFY = 1,
    ADD = 2,
    DELETE = 3,
};

//表的字段顺序信息
typedef struct fieldSquence
{
    QVector<quint16> vNLevels;         //深度队列
    QVector<QString> vSFieldSquences;       //对应的字段顺序
}FIELDSQUENCE;

//键值对
typedef struct oneValuePair
{
    QString sField;
    QString sValue;
}VALUEPAIR;

//一行的数据
typedef struct oneRowData
{
    qint16 id;
    QVector<VALUEPAIR> dataList;
}ROWDATA;

//表的数据
typedef struct tableData
{
    QString sTableName;
    quint16 nRow;
    quint16 nColumn;
    QVector<ROWDATA> dataList;
}TABLEDATA;

class TabWidgetCell : public QWidget
{
    Q_OBJECT

public:
    explicit TabWidgetCell(QWidget *parent = nullptr);
    ~TabWidgetCell();

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

    void SetProtoData(test_2::table_data& proto);

    //请求保存数据
    void OnRequestSaveData();

    inline bool IsHeadIndexChange()
    {
        return m_bHeadIndexChange;
    }

    inline bool IsTableDataChange()
    {
        return m_bTableDataChange;
    }
protected:
    virtual void keyPressEvent(QKeyEvent *ev);
private:
    //设置数据有改变
    void SetDataModify(bool modify);
private slots:
    void OnItemDataChange(QStandardItem *item);

    void sectionMovableBtnClicked();

    //移动列
    void OnTableViewSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);

    //刷新界面
    void Flush();
private:
    Ui::TabWidgetCell *ui;

    QTabWidget* m_tabWidget;    //所在的tabwidget
    MainWindow* m_mainWindow;    //所在的mainWindow

    QWidget*    m_topWidget;   //上面的widget

    QTableView*   m_tableView;  //展示数据的二维表
    QListWidget*    m_rightButtonList;  //右边的操作按钮

    QHBoxLayout* hlayout_top;  //上面的水平布局

    QListWidget*    m_bottomButtonList; //底部的显示按钮列表
    QVBoxLayout* vlayout_all;  //整个TabWidget类的垂直布局

    TABLEDATA   m_tableData;    //表的数据
    QVector<QString> m_mFieldLists;  //字段的列表

    bool    m_bHeadIndexChange; //表头顺序是否被更改
    bool    m_bTableDataChange; //表的数据是否被更改
    QMap<QString, int>  m_mFieldNames; //表的字段的顺序
    QMap<QString, int>  m_mFieldTypes; //表的字段对应的类型

    QVector<RowState>   m_vBRowDataChange;  //行数据是否被改变(数据变化存储以二维表的行为粒度)
};

#endif // TABWIDGETCELL_H

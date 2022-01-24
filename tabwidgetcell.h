#ifndef TABWIDGETCELL_H
#define TABWIDGETCELL_H

#include <QWidget>
#include <QHBoxLayout>
#include <QTableView>
#include <QListWidget>
#include <QStandardItemModel>
#include <QPushButton>
#include <QDebug>
#include "msg.pb.h"
#include <google/protobuf/text_format.h>

namespace Ui {
class TabWidgetCell;
}

class TabWidgetCell : public QWidget
{
    Q_OBJECT

public:
    explicit TabWidgetCell(QWidget *parent = nullptr);
    ~TabWidgetCell();

    void SetProtoData(test_2::table_data& proto);

    inline bool IsHeadIndexChange()
    {
        return m_bHeadIndexChange;
    }
private slots:
    void sectionMovableBtnClicked();

    //移动列
    void OnTableViewSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);
private:
    Ui::TabWidgetCell *ui;
    QWidget*    m_topWidget;   //上面的widget

    QTableView*   m_tableView;  //展示数据的二维表
    QListWidget*    m_rightButtonList;  //右边的操作按钮

    QHBoxLayout* hlayout_top;  //上面的水平布局

    QListWidget*    m_bottomButtonList; //底部的显示按钮列表
    QVBoxLayout* vlayout_all;  //整个TabWidget类的垂直布局

    bool    m_bHeadIndexChange; //表头顺序是否被更改
    QMap<QString, int>  m_mFieldNames; //表的字段的顺序
    QMap<QString, int>  m_mFieldTypes; //表的字段对应的类型
};

#endif // TABWIDGETCELL_H

#ifndef LUATABLEDATAWIDGET_H
#define LUATABLEDATAWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>

#include "msg.pb.h"
#include "tabwidgetcell.h"

typedef struct fieldInfo
{
    QString sFieldName;      //字段名字
    QString sFieldAnnonation;      //字段的备注
    QString sFieldLink;      //字段的关联
}FIELDINFO;

//表的字段信息
typedef struct fieldSquence
{
    QVector<quint16> vNLevels;         //深度队列
    QVector<FIELDINFO> vSFieldSquences;       //对应的字段顺序
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

//lua的二维数据表界面
class LuaTableDataWidget : public TabWidgetCell
{
    Q_OBJECT

public:
    LuaTableDataWidget(QWidget *parent = nullptr);
    virtual ~LuaTableDataWidget() {};

    inline bool IsHeadIndexChange()
    {
        return m_bHeadIndexChange;
    }

    virtual inline bool IsTableDataChange()
    {
        return m_bHeadIndexChange | m_bTableDataChange;
    }

    //调整表的字段顺序
    void ModifyFieldSquences(QVector<quint16>& vNLevels, QMap<QString, quint16> mFieldSortMap);

    void SetProtoData(const test_2::table_data& proto);

    virtual void OnRequestSaveData();

    virtual void GlobalKeyPressEevent(QKeyEvent *ev);

    const QVector<FIELDINFO>& GetFieldInfos(QVector<quint16> vNLevels);

    FIELDINFO& GetFieldInfos(QVector<quint16> vNLevels, quint16 nIndex);
private slots:
    //移动列
    void OnTableViewSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);

    void sectionMovableBtnClicked();

    virtual void Flush();

    virtual void OnSaveAnnonations(QString str, quint32 nIndex);
private:

    TABLEDATA   m_tableData;    //表的数据
    QVector<QString> m_mFieldLists;  //字段的列表

    bool    m_bHeadIndexChange; //表头顺序是否被更改
    QMap<QString, int>  m_mFieldNames; //表的字段的顺序
    QMap<QString, int>  m_mFieldTypes; //表的字段对应的类型

    QVector<FIELDSQUENCE>   m_vFieldSquence;   //二维表的表头顺序

//    QVector<RowState>   m_vBRowDataChange;  //行数据是否被改变(数据变化存储以二维表的行为粒度)
};

#endif // LUATABLEDATAWIDGET_H

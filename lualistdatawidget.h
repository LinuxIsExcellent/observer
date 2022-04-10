#ifndef LUALISTDATAWIDGET_H
#define LUALISTDATAWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>

#include "msg.pb.h"
#include "tabwidgetcell.h"
#include "stringtotableview.h"

typedef struct FieldKeyType
{
    QString sKey;
    QString sValue;
    quint16 nType;

    friend QDebug& operator << (QDebug out, const FieldKeyType& info)
    {
        out << info.sKey << info.sValue << info.nType;

        return out;
    };
}FIELDKEYTYPE;

class LuaListDataWidget : public TabWidgetCell
{
    Q_OBJECT
public:
    LuaListDataWidget(QWidget *parent = nullptr);
    virtual ~LuaListDataWidget();

    virtual inline bool IsTableDataChange()
    {
        return m_bTableDataChange;
    }

    void SetProtoData(const test_2::send_lua_list_data_notify& proto);

    virtual void OnRequestSaveData();

    virtual void SetRowAndColParam();

    virtual void SetFieldLink(QString sIndex, QString sField, QString sFieldLink);

    void OnShowTableWithLinkMsg(QString sField, QString sValue);
public slots:
    virtual void OnItemDataChange(QStandardItem *item);
private slots:
    virtual void Flush();
private:
    QVector<FIELDKEYTYPE> m_mDataList;  //表的数据

    QMap<QString, FIELDSQUENCE>   m_mFieldSquence;   //二维表的表头顺序

    StringToTableView* m_stringToTableView;
};

#endif // LUALISTDATAWIDGET_H

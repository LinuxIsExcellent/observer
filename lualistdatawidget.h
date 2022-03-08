#ifndef LUALISTDATAWIDGET_H
#define LUALISTDATAWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>

#include "msg.pb.h"
#include "tabwidgetcell.h"

typedef struct FieldKeyType
{
    QString sKey;
    QString sValue;
    quint16 nType;
}FIELDKEYTYPE;

class LuaListDataWidget : public TabWidgetCell
{
    Q_OBJECT
public:
    LuaListDataWidget(QWidget *parent = nullptr);
    virtual ~LuaListDataWidget(){};

    virtual inline bool IsTableDataChange()
    {
        return m_bTableDataChange;
    }

    void SetProtoData(const test_2::send_lua_list_data_notify& proto);

    virtual void OnRequestSaveData();
private slots:
    virtual void Flush();
private:
    QVector<FIELDKEYTYPE> m_mDataList;  //表的数据
};

#endif // LUALISTDATAWIDGET_H

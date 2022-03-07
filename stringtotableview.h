#ifndef STRINGTOTABLEVIEW_H
#define STRINGTOTABLEVIEW_H

#include <QDialog>
#include <QDebug>
#include <QStandardItemModel>
#include <QMenu>

extern "C"
{
     #include "lua.h"
     #include "lauxlib.h"
     #include "lualib.h"
}

typedef struct RowInfo
{
    QString sKey;      //字段名
    QString sField;      //字段值
    int     nType;      //值的类型
}ROWINFO;

namespace Ui {
class StringToTableView;
}

class StringToTableView : public QDialog
{
    Q_OBJECT

public:
    explicit StringToTableView(QWidget *parent = nullptr, int nLevel = 0);
    void SetParam(int nCol, int nRow, QString str);

    ~StringToTableView();

    void Flush();

    void OnSaveData();

    std::string ParseLuaTableToString(lua_State *L);
private slots:
    void OnCancelButtonClicked();
    void OnConfirmButtonClicked();

    void OnItemDataChange(QStandardItem * item);
private:
    Ui::StringToTableView *ui;

    QStandardItemModel* m_standardItemModel;

    QMenu*  m_tableCellMenu;        //二维表数据的菜单栏

    QVector<ROWINFO>    m_vRowDatas;    //数据

    bool        m_bDataChange;      //数据是否改变

    int         m_nLevel;       //多少级

    int         m_nRow;     //行
    int         m_nCol;     //列
};

#endif // STRINGTOTABLEVIEW_H

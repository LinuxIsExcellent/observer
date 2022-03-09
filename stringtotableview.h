#ifndef STRINGTOTABLEVIEW_H
#define STRINGTOTABLEVIEW_H

#include <QDialog>
#include <QDebug>
#include <QStandardItemModel>
#include <QMenu>
#include <QUndoStack>
#include <QUndoView>
#include <QAction>

extern "C"
{
     #include "lua.h"
     #include "lauxlib.h"
     #include "lualib.h"
}

typedef struct RowInfo
{
    QString sKey;      //字段名
    int     nKeyType;      //字段的类型
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
    explicit StringToTableView(QStandardItemModel *model, QModelIndex index, QWidget *parent = nullptr, int nLevel = 0);

    ~StringToTableView();

    void SetParam();

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

    QStandardItemModel *model;  //对应的tableView的model
    QModelIndex index;      //tableView的modelindex
    QString m_sData;        //原本的dataStr

    //实现撤销功能
    QAction *undoAction;
    QAction *redoAction;
    QUndoStack *undoStack;
    QUndoView *undoView;
};

#endif // STRINGTOTABLEVIEW_H

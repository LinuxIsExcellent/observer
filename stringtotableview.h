#ifndef STRINGTOTABLEVIEW_H
#define STRINGTOTABLEVIEW_H

#include <QDialog>
#include <QDebug>
#include <QStandardItemModel>
#include <QMenu>
#include <QUndoStack>
#include <QUndoView>
#include <QAction>
#include "luatabledatawidget.h"

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

    friend QDebug& operator << (QDebug out, const RowInfo& info)
    {
        out << info.sKey << info.nKeyType << info.sField << info.nType;

        return out;
    };
}ROWINFO;

namespace Ui {
class StringToTableView;
}

class StringToTableView : public QDialog
{
    Q_OBJECT

public:
    explicit StringToTableView(QStandardItemModel *model, QModelIndex index, QString sTableName, QMap<QString, FIELDSQUENCE>* pMFieldSquence, TabWidgetCell* cellWidget, QString sTitleName, QWidget *parent = nullptr, int nLevel = 0);

    ~StringToTableView();

    void CheckItemDataTypeIsCorrect(QStandardItem *item);

    void OnChangeBtnState();

    void SetParam();

    void Flush();

    bool OnSaveData();

    void OnChangeData();

    void clearUndoStack()
    {
        undoStack->clear();
    }

    void SetDataModify()
    {
        m_bDataChange = true;
    }

    void SetFieldDataModify()
    {
        m_bFieldSquenceChange = true;
    }

    //改变一个modelIndex的数据
    void ChangeModelIndexData(QModelIndex index, QString sData);

    //撤销
    void undo();

    //返回撤销
    void redo();

    //复制单元格的内容
    void copy();

    //粘贴全局剪切板的内容
    void paste();

protected:
    void closeEvent(QCloseEvent *event);

    virtual void keyPressEvent(QKeyEvent *ev);

    bool eventFilter(QObject *obj, QEvent *eve);
private slots:
    std::string ParseLuaTableToString(lua_State *L, QString sTableKey);

    void OnCancelButtonClicked();
    void OnConfirmButtonClicked();

    void OnItemDataChange(QStandardItem * item);
private:
    Ui::StringToTableView *ui;

    QStandardItemModel* m_standardItemModel;

    TabWidgetCell* m_cellWidget;    //属于哪一个表

    QMenu*  m_tableCellMenu;        //二维表数据的菜单栏

    QMap<QString, FIELDSQUENCE>* m_mFieldSquence;    //表头数据

    QVector<ROWINFO>    m_vRowDatas;    //数据
    QMap<QString, int>    m_vRowDataTypes;    //键值对的类型
    QMap<int, bool>             m_mTypeCheck;       //存储错误格式的类型的单元格row * 10000 + col, 是否出错

    QString m_sTableName;       //该展开界面在额外信息表内的表名
    QString m_sTitleName;       //标题栏

    bool        m_bDataChange;      //数据是否改变
    bool        m_bFieldSquenceChange;     //表的额外信息是否改变

    int         m_nLevel;       //多少级

    QStandardItemModel *model;  //对应的tableView的model
    QModelIndex m_index;      //tableView的modelindex
    QString m_sData;        //原本的dataStr

    //实现撤销功能
    QAction *undoAction;
    QAction *redoAction;
    QUndoStack *undoStack;
    QUndoView *undoView;
};

#endif // STRINGTOTABLEVIEW_H

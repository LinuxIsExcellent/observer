#include "stringtotableview.h"
#include "ui_stringtotableview.h"

StringToTableView::StringToTableView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StringToTableView)
{
    ui->setupUi(this);

    setWindowTitle("表");
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setModal(true);
}

StringToTableView::~StringToTableView()
{
    delete ui;
}

void StringToTableView::SetParam(int nCol, int nRow, QString str)
{
    lua_State *L = luaL_newstate();
    if (L == NULL) return;

    str = "temp_table = " + str;
    int ret = luaL_dostring(L, str.toStdString().c_str());
    if (ret)
    {
        qCritical() << lua_tostring(L,-1);
    }

    lua_getglobal(L, "temp_table");
//    ParseLuaTable(L);

    if (!lua_istable(L, -1))
    {
        qDebug() << "temp_table is not a lua table";
        return;
    }

    //置空栈顶
    lua_pushnil(L);

    while(lua_next(L, -2))
    {

        if (lua_type(L, -2) == LUA_TNUMBER)
        {
            int nKey = lua_tonumber(L, -2);

            qDebug() << "nKey = " << nKey;
        }
        else if (lua_type(L, -2) == LUA_TSTRING)
        {
            QString sKey = lua_tostring(L, -2);
            qDebug() << "sKey = " << sKey;
        }

//        qDebug() << "lua_type = " << lua_type(L, -1);

        lua_pop(L, 1);
    }


}

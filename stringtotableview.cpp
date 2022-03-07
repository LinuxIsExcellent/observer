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

std::string StringToTableView::ParseLuaTableToString(lua_State *L)
{
    if (L == NULL) return "";

    std::string sValueTable = "{";
    if (!(lua_type(L, -1) == LUA_TTABLE))
    {
        qDebug () << "is not a string";
        return "";
    }

    lua_pushnil(L);
    bool has_field = false;
    // LOG_INFO("lua_rawlen - " + std::to_string(lua_rawlen(L, -2)));
    while(lua_next(L, -2))
    {
        std::string sKey = "";

        if (lua_type(L, -2) == LUA_TNUMBER || lua_type(L, -2) == LUA_TNIL)
        {
            sKey = "[" + std::to_string(lua_tointeger(L, -2)) + "]";
        }
        else if (lua_type(L, -2) == LUA_TSTRING)
        {
            std::string strKey = lua_tostring(L, -2);
            if (strKey.find_first_not_of("-.0123456789") == std::string::npos)
            {
                sKey = std::string("[\"") + lua_tostring(L, -2) + std::string("\"]");
            }
            else
            {
                sKey = lua_tostring(L, -2);
            }
        }

        sValueTable = sValueTable + sKey + " = ";

        // 如果key值是一个table
        if (lua_type(L, -1) == LUA_TTABLE)
        {
            sValueTable = sValueTable + ParseLuaTableToString(L);
        }
        else if (lua_type(L, -1) == LUA_TSTRING)
        {
            sValueTable = sValueTable + std::string("\"") + lua_tostring(L, -1) + std::string("\"");
            // cout << "sValue = " << lua_tostring(L, -1) << endl;
        }
        else if (lua_type(L, -1) == LUA_TBOOLEAN)
        {
            sValueTable = sValueTable + std::to_string(lua_toboolean(L, -1));
            // cout << "sValue = " << lua_toboolean(L, -1) << endl;
        }
        else if (lua_type(L, -1) == LUA_TNIL)
        {
            sValueTable = sValueTable + std::to_string(lua_tointeger(L, -1));
        }
        else if (lua_type(L, -1) == LUA_TNUMBER)
        {
            double num = lua_tonumber(L, -1);
            std::string str_num = doubleToString(num);
            sValueTable = sValueTable + str_num;
            // sValueTable = sValueTable + std::to_string(num);
        }

        sValueTable = sValueTable + ", ";
        lua_pop(L, 1);

        has_field = true;
    }

    // 最后一个", "要去掉，暂时找不到怎么判断lua_next中的元素全部遍历结束的方法，如果有办法判断可以在lua_next里面加方法处理
    if(has_field)
    {
        sValueTable = sValueTable.erase(sValueTable.length() - 2, 2);
    }

    sValueTable = sValueTable + "}";
    return sValueTable;
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

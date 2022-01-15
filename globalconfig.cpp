#include <QDebug>
#include "globalconfig.h"
#include "log.h"

GlobalConfig::GlobalConfig()
{

}

bool GlobalConfig::LoadConfig(lua_State* L, QString fileName)
{
    QDir dir;
    QString currentPath = dir.currentPath();
    const char* luaPathStr = QString(currentPath + "/" + fileName).toStdString().c_str();

    if (!L) return false;

    int ret = luaL_dofile(L, luaPathStr);
    if (ret)
    {
        qCritical() << lua_tostring(L,-1);
        return false;
    }

    lua_getglobal(L, "Program");
//    ParseLuaTable(L);

    if (!lua_istable(L, -1))
    {
        qDebug() << "Program is not a lua table";
        return false;
    }

    //置空栈顶
    lua_pushnil(L);

    //定制化结构读取，没有必要正对table进行递归，两层table结构
    while(lua_next(L, -2))      //把表里面的key-value循环压入栈中
    {
        // key : 项目名字   value : 项目信息
        QString programStr = lua_tostring(L, -2);
//        qDebug() << "key  = " << programStr;

        if (lua_type(L, -1) == LUA_TTABLE)
        {
            sProgram program;

            lua_pushnil(L);
            while(lua_next(L, -2))
            {
                QString sKeyName = lua_tostring(L, -2);
//                qDebug() << "sKeyName = " << sKeyName;
                if(sKeyName == "name")
                {
                    QString sValueName = lua_tostring(L, -1);
//                    qDebug() << "sValueName = " << sValueName;
                    program.sProgramName = sValueName;
                }
                else if (sKeyName == "serverLists")
                {
                    if (lua_type(L, -1) == LUA_TTABLE)
                    {
                        lua_pushnil(L);

//                        int serverCount = lua_rawlen(L, -2);=
                        while(lua_next(L, -2))
                        {
                            if (lua_type(L, -1) == LUA_TTABLE)
                            {
                                sServerInfo serverInfo;

                                lua_pushnil(L);
                                while(lua_next(L, -2))
                                {
//                                    qDebug() << "server param key = " << lua_tostring(L, -2);
//                                    qDebug() << "server param value = " << lua_tostring(L, -1);
                                    QString fieldName = lua_tostring(L, -2);
                                    if (fieldName == "name")
                                    {
                                        serverInfo.name = lua_tostring(L, -1);
                                    }
                                    else if (fieldName == "ip")
                                    {
                                        serverInfo.ip = lua_tostring(L, -1);
                                    }
                                    else if (fieldName == "port")
                                    {
                                        serverInfo.port = lua_tonumber(L, -1);
                                    }
                                    else
                                    {
                                        qDebug() << "unknow key = " << fieldName << ", value = " << lua_tostring(L, -1);
                                    }

                                    lua_pop(L, 1);
                                }
                                program.serverList.push_back(serverInfo);
                            }

                            lua_pop(L, 1);
                        }

//                        qDebug () << program;
                    }
                }

                lua_pop(L, 1);
            }

            m_programs.insert(programStr, program);

//            qDebug() << m_programs;
        }
        else
        {
            return false;
        }

        lua_pop(L, 1);
    }

    return true;
}

void GlobalConfig::ParseLuaTable(lua_State *L)
{
    if (!lua_istable(L, -1))
    {
        return;
    }

    lua_pushnil(L);


    while (lua_next(L, -2))
    {
        fprintf(stdout, "%s : %s    ", luaL_typename(L,-2), luaL_typename(L,-1));
        int nKeyType = lua_type(L, -2);
        int nValueType = lua_type(L, -1);

        if (nKeyType == LUA_TNUMBER)
        {
            fprintf(stdout, "%g,", lua_tonumber(L, -2));
        }
        else if (nKeyType == LUA_TSTRING)
        {
            fprintf(stdout, "\"%s\",", lua_tostring(L, -2));
        }
        fprintf(stdout, "   ");
        if (nValueType == LUA_TNUMBER)
        {
            fprintf(stdout, "%g", lua_tonumber(L, -1));
        }
        else if (nValueType == LUA_TSTRING)
        {
            fprintf(stdout, "%s", lua_tostring(L, -1));
        }
        else if (nValueType == LUA_TTABLE)
        {
            fprintf(stdout, "\n");
            ParseLuaTable(L);
        }

//        qDebug() << "key value = " << lua_tonumber(L, -2);
//        qDebug() << "value1 type = " << luaL_typename(L, -1);
//        qDebug() << "stack counts = " << lua_gettop(L);

        lua_pop(L, 1);
        fprintf(stdout, "\n");
    }

    fflush(stdout);
}

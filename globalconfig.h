#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H
#include <QString>
#include <QDir>
#include <QMap>
#include <QVector>
#include <QTextCodec>
#include <QDebug>

extern "C"
{
     #include "lua.h"
     #include "lauxlib.h"
     #include "lualib.h"
}


//服务器
struct sServerInfo
{
    QString     name;
    QString     ip;
    int         port;

    friend QDebug& operator<<(QDebug out, const sServerInfo& info)
    {
        out << info.name << info.ip << info.port;
        return out;
    }
};

//项目
struct sProgram
{
    QString      sProgramName;
    QVector<sServerInfo>    serverList;

    friend QDebug& operator<<(QDebug out, const sProgram& info)
    {
        out << info.sProgramName << info.serverList;
        return out;
    }
};

typedef QMap <QString, sProgram> qMapPrograms;

class GlobalConfig
{
private:
    GlobalConfig();
public:
    ~GlobalConfig() {};

    static GlobalConfig* getInstance(){
        if (m_instance == nullptr)
        {
            m_instance = new GlobalConfig();
        }

        return m_instance;
    }

//    friend QDebug& operator<<(QDebug out, const qMapPrograms& info)
//    {
//        qMapPrograms::const_iterator it;
//        for(it = info.begin(); it != info.end(); ++it)
//        {
//            out << it.key() << it.value();
//        }
//        return out;
//    }

    bool LoadConfig(lua_State* L, QString fileName);

    void ParseLuaTable(lua_State* L);

    const qMapPrograms* GetProgramData()
    {
        return &m_programs;
    }

    qMapPrograms::const_iterator GetProgramInfoList(QString& sProgram)
    {
        return m_programs.find(sProgram);
    }
private:
    static GlobalConfig* m_instance;

    qMapPrograms  m_programs;
};

#endif // GLOBALCONFIG_H

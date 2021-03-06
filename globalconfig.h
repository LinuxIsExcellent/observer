#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H
#include <QString>
#include <QDir>
#include <QMap>
#include <QVector>
#include <QTextCodec>
#include <QDebug>
#include <regex>

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

    friend QDebug& operator<<(QDebug& out, const sServerInfo& info)
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

    friend QDebug& operator<<(QDebug& out, const sProgram& info)
    {
        out << info.sProgramName << info.serverList;
        return out;
    }
};

typedef QMap <QString, sProgram> qMapPrograms;

class MainWindow;

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

    bool LoadConfig(QString fileName);

    void ParseLuaTable(lua_State* L);

    void SetMainWindow(MainWindow* mainWindow)
    {
        m_mainWindow = mainWindow;
    }

    MainWindow* GetMainWindow()
    {
        return m_mainWindow;
    }

    const qMapPrograms* GetProgramData()
    {
        return &m_programs;
    }

    qMapPrograms::const_iterator GetProgramInfoList(QString& sProgram)
    {
        return m_programs.find(sProgram);
    }

    std::string doubleToString(double price);

    bool CheckStrIsCorrectType(QString str, int nType);

    int PickStrALuaValueType(QString str);
private:
    static GlobalConfig* m_instance;

    qMapPrograms  m_programs;
    MainWindow*   m_mainWindow;
};

#endif // GLOBALCONFIG_H

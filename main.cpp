#include "mainwindow.h"
#include "parseconfig.h"
#include "globalconfig.h"
#include <QApplication>
#include <QHash>
#include <QDataStream>
#include <iostream>
using namespace std;


extern "C"
{
     #include "lua.h"
     #include "lauxlib.h"
     #include "lualib.h"
}

ParseConfig* ParseConfig::m_instance = NULL;
GlobalConfig* GlobalConfig::m_instance = NULL;
static const QString qLConfigPath = "config/globalConfig.lua";

//全局唯一的虚拟机
static lua_State *L = NULL;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    L = luaL_newstate();
    if (L ==NULL)
    {
        return 0;
    }

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);

    #ifdef QT_NO_DEBUG
        logSysInit("log");
    #endif

//    logSysInit("log");

    GlobalConfig::getInstance()->LoadConfig(L, qLConfigPath);

//    QMap<QString, QString> program_map;
//    program_map.insert("ph", "菲律宾");
//    program_map.insert("th", "泰国");

//    ParseConfig::getInstance()->updateValue("program", program_map);

//    program_map.clear();
//    program_map.insert("server1", "192.168.1.62");
//    program_map.insert("server2", "192.168.1.65");
//    ParseConfig::getInstance()->updateValue("朱迪胜", program_map);
    w.init_windows();
    w.show();
    return a.exec();
}

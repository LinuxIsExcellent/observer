#include "mainwindow.h"
#include "logindialog.h"
#include "parseconfig.h"
#include "globalconfig.h"
#include <QApplication>
#include <QHash>
#include <QDataStream>
#include <iostream>
#include <QObject>
#include <log.h>

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
    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow w;

    L = luaL_newstate();
    if (L ==NULL)
    {
        return 0;
    }

    QString path = QCoreApplication::applicationDirPath ();

    qDebug() << path;
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);

    #ifdef QT_NO_DEBUG
        logSysInit("log");
    #endif

//    logSysInit("log");

    GlobalConfig::getInstance()->LoadConfig(L, qLConfigPath);

    LoginDialog loginDialog(&w);

    loginDialog.show();

    w.SetLoginDialog(&loginDialog);

//    w.show();
    return a.exec();
}

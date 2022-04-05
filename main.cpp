#include "globalapplication.h"
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

int main(int argc, char *argv[])
{
    GlobalApplication a(argc, argv);
    GlobalApplication::setQuitOnLastWindowClosed(false);

    MainWindow w;
    QString path = QCoreApplication::applicationDirPath ();

//    qDebug() << path;
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);

    #ifdef QT_NO_DEBUG
        logSysInit("log");
    #endif

    GlobalConfig::getInstance()->LoadConfig(qLConfigPath);
    GlobalConfig::getInstance()->SetMainWindow(&w);

    w.SetLoginDialog();

    return a.exec();
}

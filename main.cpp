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

    std::string s1 = std::string("string1");
    std::string s2 = std::string("string2");

    const char* p1 = s1.substr(1).data();
    const char* p2 = s2.substr(1).data();

    qDebug() << "p1 = " << p1 << p2;

//    QString qs1("asdas");
//    QString qs2("12321");
//    const char* s1 = qs1.toStdString().data();
//    const char* s2 = qs2.toStdString().data();
//    qDebug() << "object to ptr : " << s1;
//    qDebug() << "";
//    qDebug() << "object to ptr : " << s2;

    QString path = QCoreApplication::applicationDirPath ();

    qDebug() << path;
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);

    #ifdef QT_NO_DEBUG
        logSysInit("log");
    #endif

//    logSysInit("log");

    GlobalConfig::getInstance()->LoadConfig(qLConfigPath);

    LoginDialog loginDialog(&w);

    loginDialog.show();

    w.SetLoginDialog(&loginDialog);

//    w.show();
    return a.exec();
}

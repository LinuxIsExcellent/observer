#ifndef LOG_H
#define LOG_H


#include <QFile>
#include <QMessageLogger>
#include <qlogging.h>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>

extern QMessageLogger *gMLog;
void logSysInit();
void logSysInit(QString filePath);

QFile * getLogFile(QtMsgType type);

//打印日志到文件中
//qDebug("This is a debug message");
//qWarning("This is a warning message");
//qCritical("This is a critical message");
//qFatal("This is a fatal message");

#endif // LOG_H

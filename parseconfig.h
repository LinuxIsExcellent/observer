#ifndef PARSECONFIG_H
#define PARSECONFIG_H
#include <QString>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QMapIterator>
#include <QTextCodec>

class ParseConfig
{
public:
    ~ParseConfig();

        static ParseConfig* getInstance()
        {
            if (m_instance == NULL)
            {
                m_instance = new ParseConfig();
            }

            return m_instance;
        }

    private:
        ParseConfig();
    public:
        QString getStrValueByKey(QString qStrKey);

        int getIntValueByKey(QString qStrKey);

        void updateValue(QString qStrKey, QVariant qStrValue);

        void updateValue(QString qStrKey, QMap<QString, QString>& map);

        QStringList getGroupKeys(QString qStrGroup);
    private:
        QSettings* m_settings;
        static ParseConfig* m_instance;
};

#endif // PARSECONFIG_H

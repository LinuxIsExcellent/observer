#include "parseconfig.h"

static const QString qSConfigPath = "config/test.ini";

ParseConfig::ParseConfig()
{
    QDir dir;
    QString currentPath = dir.currentPath();

    m_settings = new QSettings(currentPath + "/" + qSConfigPath, QSettings::IniFormat);
    m_settings->setIniCodec(QTextCodec::codecForName("utf-8"));
}


ParseConfig::~ParseConfig()
{
}

QString ParseConfig::getStrValueByKey(QString qStrKey)
{
    if (!m_settings || !m_settings->contains(qStrKey))
        return "";
    return m_settings->value(qStrKey).toString();
}

int ParseConfig::getIntValueByKey(QString qStrKey)
{
    if (!m_settings || !m_settings->contains(qStrKey))
        return 0;
    return  m_settings->value(qStrKey).toInt();
}

void ParseConfig::updateValue(QString qStrKey, QVariant qStrValue)
{
    if (!m_settings) return;
    m_settings->setValue(qStrKey, qStrValue);
}

void ParseConfig::updateValue(QString qStrKey, QMap<QString, QString>& map)
{
    if (!m_settings) return;

    m_settings->beginGroup(qStrKey);

    auto iter = map.begin();
    while (iter != map.end())
    {
        m_settings->setValue(iter.key(), iter.value());
        iter++;
    }

    m_settings->endGroup();
}

QStringList ParseConfig::getGroupKeys(QString qStrGroup)
{
    QStringList list;
    if (!m_settings) return list;

    m_settings->beginGroup(qStrGroup);

    list = m_settings->childKeys();

    m_settings->endGroup();

    return list;
}

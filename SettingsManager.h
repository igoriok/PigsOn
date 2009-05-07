#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include "SAccount.h"

class SettingsManager : public QObject
{
    Q_OBJECT
public:
    SettingsManager(QObject * parent = 0);
    ~SettingsManager();

public slots:
    void loadConfig();
    void saveConfig();
    void showDialog();

signals:
    void settingsChanged();

public:
    QVariant getOption(const QString & name) const;
    void setOption(const QString & name, const QVariant & value);

private:
    QMap<QString, QVariant> _settings;
};

#endif // SETTINGSMANAGER_H

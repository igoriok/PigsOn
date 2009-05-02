#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
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
    Account getPigsAccount() const { return pigsAccount; }
    Account getHostopAccount() const { return hostopAccount; }
    QList<int> getPigsGroups() const { return pigsGroups; }
    int getTimerInterval() const { return timerInterval; }
    QByteArray getMainWindowState() const { return mainWindowState; }
    bool getLang() const { return ru_lang; }
    bool showTrayIcon() const { return trayIcon; }
    bool showTrayMessages() const { return trayMessages; }

    void setPigsGroups(QList<int> & groups) { pigsGroups = groups; }
    void setMainWindowState(const QByteArray & state) { mainWindowState = state; }
    void setLang(bool lang) { ru_lang = lang; }

private:
    Account pigsAccount;
    Account hostopAccount;
    QList<int> pigsGroups;
    int timerInterval;
    QByteArray mainWindowState;
    bool ru_lang;

    bool trayIcon;
    bool trayMessages;
};

#endif // SETTINGSMANAGER_H

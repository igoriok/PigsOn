#include <QtCore/QSettings>
#include "SettingsManager.h"
#include "SettingsDialog.h"

SettingsManager::SettingsManager(QObject * parent) : QObject(parent)
{
}

void SettingsManager::loadConfig()
{
    QSettings sett;
    QString groups;
    sett.beginGroup("PigsAccount");
    pigsAccount.user = sett.value("username", QString()).toString();
    pigsAccount.pass = sett.value("password", QString()).toString();
    groups = sett.value("groups", QString()).toString();
    if (!groups.isEmpty())
    {
        QStringList gr = groups.split(':');
        for(int i = 0; i < gr.size(); ++i)
            if (gr.at(i).toInt() != 0)
                pigsGroups.append(gr.at(i).toInt());
    }
    timerInterval = sett.value("interval", int(0)).toInt();
    sett.endGroup();

    sett.beginGroup("HostopAccount");
    hostopAccount.user = sett.value(QString("username"), QString()).toString();
    hostopAccount.pass = sett.value(QString("password"), QString()).toString();
    sett.endGroup();

    sett.beginGroup("Gui");
    mainWindowState = sett.value("mainWindowState", QByteArray()).toByteArray();
    trayIcon = sett.value("trayicon", false).toBool();
    trayMessages = sett.value("traymess", false).toBool();
    if (sett.value("lang", QString()).toString() == QString("RU"))
        ru_lang = true;
    else
        ru_lang = false;
    sett.endGroup();

    emit settingsChanged();
}

void SettingsManager::saveConfig()
{
    QSettings sett;
    sett.clear();
    if (pigsAccount.isValid()) {
        QString groups;
        for (int i = 0; i < pigsGroups.size(); ++i)
            groups.append(QString::number(pigsGroups.at(i))).append(":");
        if (groups.endsWith(':')) groups.remove(groups.size() - 1, 1);

        sett.beginGroup("PigsAccount");
        sett.setValue("username", pigsAccount.user);
        sett.setValue("password", pigsAccount.pass);
        sett.setValue("groups", groups);
        sett.setValue("interval", timerInterval);
        sett.endGroup();
    }

    if (hostopAccount.isValid())
    {
        sett.beginGroup("HostopAccount");
        sett.setValue(QString("username"), hostopAccount.user);
        sett.setValue(QString("password"), hostopAccount.pass);
        sett.endGroup();
    }

    sett.beginGroup("Gui");
    sett.setValue("mainWindowState", mainWindowState);
    sett.setValue("trayicon", trayIcon);
    sett.setValue("traymess", trayMessages);
    if (ru_lang)
        sett.setValue("lang", QString("RU"));
    sett.endGroup();
}

void SettingsManager::showDialog()
{
    SettingsDialog dlg((QWidget *)this->parent());
    dlg.setPigsAccount(pigsAccount);
    dlg.setHostopAccount(hostopAccount);
    dlg.setTimerInterval(timerInterval);
    dlg.setTrayIcon(trayIcon);
    dlg.setTrayMess(trayMessages);
    if (dlg.exec() == QDialog::Accepted)
    {
        if (dlg.getPigsAccount().isValid()) pigsAccount = dlg.getPigsAccount();
        else pigsAccount.clear();
        if (dlg.getHostopAccount().isValid()) hostopAccount = dlg.getHostopAccount();
        else hostopAccount.clear();
        timerInterval = dlg.getTimerInterval();
        trayIcon = dlg.getTrayIcon();
        trayMessages = dlg.getTrayMess();
        emit settingsChanged();
    }
}

SettingsManager::~SettingsManager()
{
}

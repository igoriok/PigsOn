#include <QtCore/QSettings>
#include "SettingsManager.h"
#include "SettingsDialog.h"

/*
    PigsAccount
        pUsername - username    QString
        pPassword - password    QString
        pInterval - interval    int
        pGroups   - groups      QList<int>

    HostopAccoung
        hUsername - username    QString
        hPassword - password    QString

    Gui
        mwState - Main Window state     QByteArray
        tIcon   - Show Tray Icon        bool
        tMess   - Show Tray Message     bool
        iLang   - Interface Language    QString
*/

SettingsManager::SettingsManager(QObject * parent) : QObject(parent)
{
}

void SettingsManager::loadConfig()
{
    QSettings sett;

    sett.beginGroup("PigsAccount");
    QStringList keys(sett.allKeys());
    for (QStringList::const_iterator iter = keys.constBegin(); iter != keys.constEnd(); ++iter)
        _settings.insert(*iter, sett.value(*iter));
    sett.endGroup();

    sett.beginGroup("HostopAccount");
    keys = QStringList(sett.allKeys());
    for (QStringList::const_iterator iter = keys.constBegin(); iter != keys.constEnd(); ++iter)
        _settings.insert(*iter, sett.value(*iter));
    sett.endGroup();

    sett.beginGroup("Gui");
    keys = QStringList(sett.allKeys());
    for (QStringList::const_iterator iter = keys.constBegin(); iter != keys.constEnd(); ++iter)
        _settings.insert(*iter, sett.value(*iter));
    sett.endGroup();

    emit settingsChanged();
}

void SettingsManager::saveConfig()
{
    QSettings sett;
    sett.clear();

    sett.beginGroup("PigsAccount");
    QString user(_settings.value(QString("pUsername")).toString());
    QString pass(_settings.value(QString("pPassword")).toString());
    if (!user.isEmpty() && !pass.isEmpty())
    {
        sett.setValue(QString("pUsername"), user);
        sett.setValue(QString("pPassword"), pass);
        sett.setValue(QString("pInterval"), _settings.value(QString("pInterval")));
        sett.setValue(QString("pGroups"), _settings.value(QString("pGroups")));
    }
    sett.endGroup();

    sett.beginGroup("HostopAccount");
    user = _settings.value(QString("hUsername")).toString();
    pass = _settings.value(QString("hPassword")).toString();
    if (!user.isEmpty() && !pass.isEmpty())
    {
        sett.setValue(QString("hUsername"), user);
        sett.setValue(QString("hPassword"), pass);
    }
    sett.endGroup();

    sett.beginGroup("Gui");
    sett.setValue(QString("mwState"), _settings.value(QString("mwState")));
    sett.setValue(QString("tIcon"), _settings.value(QString("tIcon")));
    sett.setValue(QString("tMess"), _settings.value(QString("tMess")));
    if (_settings.value(QString("iLang")).toString() == QString("RU"))
        sett.setValue(QString("iLang"), _settings.value(QString("iLang")));
    sett.setValue("mDebug", _settings.value("mDebug"));
    sett.endGroup();
}

void SettingsManager::showDialog()
{
    SettingsDialog dlg((QWidget *)this->parent());
    dlg.setPigsAccount(Account(_settings.value("pUsername").toString(), _settings.value("pPassword").toString()));
    dlg.setHostopAccount(Account(_settings.value("hUsername").toString(), _settings.value("hPassword").toString()));
    dlg.setTimerInterval(_settings.value("pInterval").toInt());
    dlg.setTrayIcon(_settings.value("tIcon").toBool());
    dlg.setTrayMess(_settings.value("tMess").toBool());
    dlg.setDebug(_settings.value("mDebug").toBool());
    if (dlg.exec() == QDialog::Accepted) {
        Account acc = dlg.getPigsAccount();
        if (acc.isValid()) {

            _settings.insert("pUsername", acc.user);
            _settings.insert("pPassword", acc.pass);
        } else {
            _settings.remove("pUsername");
            _settings.remove("pPassword");
        }
        acc = dlg.getHostopAccount();
        if (acc.isValid()) {
            _settings.insert("hUsername", acc.user);
            _settings.insert("hPassword", acc.pass);

        } else {
            _settings.remove("hUsername");
            _settings.remove("hPassword");
        }

        _settings.insert("pInterval", dlg.getTimerInterval());
        _settings.insert("tIcon", dlg.getTrayIcon());
        _settings.insert("tMess", dlg.getTrayMess());
        _settings.insert("mDebug", dlg.getDebug());

        emit settingsChanged();
    }
}

QVariant SettingsManager::getOption(const QString & name) const
{
    return _settings.value(name);
}

void SettingsManager::setOption(const QString & name, const QVariant & value)
{
    _settings.insert(name, value);
}

SettingsManager::~SettingsManager()
{
}

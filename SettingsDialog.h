#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui/QDialog>
#include "SAccount.h"

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(SettingsDialog)
public:
    explicit SettingsDialog(QWidget *parent = 0);
    virtual ~SettingsDialog();

    Account getPigsAccount() const;
    void setPigsAccount(Account account);
    Account getHostopAccount() const;
    void setHostopAccount(Account account);
    int getTimerInterval() const;
    void setTimerInterval(int i);
    bool getTrayIcon() const;
    void setTrayIcon(bool b);
    bool getTrayMess() const;
    void setTrayMess(bool b);
    bool getDebug() const;
    void setDebug(bool b);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::SettingsDialog *m_ui;
};

#endif // SETTINGSDIALOG_H

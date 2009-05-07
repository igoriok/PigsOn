#include <QtGui/QMenu>
#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "Ticket.h"
#include <QtGui/QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);
    m_ui->listWidget->setCurrentRow(0);
}

Account SettingsDialog::getPigsAccount() const
{
    return Account(m_ui->lineEdit_PigserUsername->text(), m_ui->lineEdit_PigserPassword->text());
}

void SettingsDialog::setPigsAccount(Account account)
{
    if (account.isValid())
    {
        m_ui->lineEdit_PigserUsername->setText(account.user);
        m_ui->lineEdit_PigserPassword->setText(account.pass);
    }
}

Account SettingsDialog::getHostopAccount() const
{
    return Account(m_ui->lineEdit_HUser->text(), m_ui->lineEdit_HPass->text());
}

void SettingsDialog::setHostopAccount(Account account)
{
    if (account.isValid())
    {
        m_ui->lineEdit_HUser->setText(account.user);
        m_ui->lineEdit_HPass->setText(account.pass);
    }
}

int SettingsDialog::getTimerInterval() const
{
    if (!m_ui->checkBox_Timer->isChecked())
        return 0;
    else
        return m_ui->spinBox_Timer->value();
}

void SettingsDialog::setTimerInterval(int i)
{
    if (i > 0)
    {
        m_ui->checkBox_Timer->setChecked(true);
        m_ui->spinBox_Timer->setValue(i);
    }
}

bool SettingsDialog::getTrayIcon() const
{
    return m_ui->checkBox_TrayIcon->isChecked();
}

void SettingsDialog::setTrayIcon(bool b)
{
    m_ui->checkBox_TrayIcon->setChecked(b);
    m_ui->checkBox_TrayMess->setEnabled(b);
}

bool SettingsDialog::getTrayMess() const
{
    if (m_ui->checkBox_TrayMess->isEnabled()) return m_ui->checkBox_TrayMess->isChecked();
    else return false;
}

void SettingsDialog::setTrayMess(bool b)
{
    m_ui->checkBox_TrayMess->setChecked(b);
}

bool SettingsDialog::getDebug() const
{
    return m_ui->checkBox_Debug->isChecked();
}

void SettingsDialog::setDebug(bool b)
{
    m_ui->checkBox_Debug->setChecked(b);
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

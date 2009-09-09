#include <QtGui/QMessageBox>
#include "TicketWidget.h"
#include "ui_TicketWidget.h"


TicketWidget::TicketWidget(int id, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::TicketWidget)
{
    m_ui->setupUi(this);

    ticket.CaseID = id;
    if (id)
        this->setEnabled(false);
}

void TicketWidget::updateContent(const Ticket & ticket)
{
    this->setEnabled(false);
    this->ticket = ticket;

    initLists();

    if (ticket.CaseID > 0)
        m_ui->label_CaseID->setText(QString("<a style=\"text-decoration: underline; color:#0000ff;\" href=\"https://support.24hourwebhostingsupport.com/showcases.php?showme=%1\">%2</a>").arg(ticket.CaseID).arg(tr("CaseID")));
    else
        m_ui->label_CaseID->setText(tr("CaseID"));

    m_ui->lineEdit_CaseID->setText(QString::number(ticket.CaseID));
    m_ui->lineEdit_DateOpened->setText(ticket.DateOpened);
    m_ui->lineEdit_DateClosed->setText(ticket.DateClosed);
    m_ui->comboBox_Hostopian->setCurrentIndex(m_ui->comboBox_Hostopian->findText(ticket.Hostopian, Qt::MatchFixedString));
    m_ui->lineEdit_Domain->setText(ticket.Domain);
    m_ui->lineEdit_CustName->setText(ticket.Name);
    m_ui->lineEdit_Email->setText(ticket.Email);
    m_ui->comboBox_Category->setCurrentIndex(m_ui->comboBox_Category->findData(QVariant(ticket.Category), Qt::UserRole, Qt::MatchFixedString));
    m_ui->comboBox_SubCategory->setCurrentIndex(m_ui->comboBox_SubCategory->findText(ticket.SubCategory, Qt::MatchFixedString));
    m_ui->plainTextEdit_Problem->setPlainText(ticket.Problem);
    m_ui->comboBox_Priority->setCurrentIndex(m_ui->comboBox_Priority->findText(QString::number(ticket.Priority)));
    m_ui->comboBox_Status->setCurrentIndex(m_ui->comboBox_Status->findData(QVariant(ticket.Status), Qt::UserRole, Qt::MatchFixedString));
    m_ui->plainTextEdit_Notes->setPlainText(ticket.Notes);
    m_ui->plainTextEdit_Resolution->setPlainText(ticket.Resolution);
    m_ui->plainTextEdit_AuditTrail->setPlainText(ticket.AuditTrail);
    m_ui->lineEdit_OpenedBy->setText(ticket.OpenedByTech);
    m_ui->lineEdit_ClosedBy->setText(ticket.ClosedByTech);
    m_ui->comboBox_Group->setCurrentIndex(m_ui->comboBox_Group->findData(QVariant(ticket.AssignedGroup), Qt::UserRole));
    m_ui->textEdit_History->setText(ticket.AuditPrev);

    m_ui->pushButton_Update->setEnabled(true);
    this->setEnabled(true);
}

void TicketWidget::initLists()
{
    m_ui->comboBox_Hostopian->clear();
    for (int i = 0; i < Ticket::Hostopians.size(); ++i)
        m_ui->comboBox_Hostopian->addItem(Ticket::Hostopians.at(i));

    m_ui->comboBox_Category->clear();
    for (QMap<int, QString>::const_iterator i = Ticket::Categories.constBegin(); i != Ticket::Categories.constEnd(); ++i)
        m_ui->comboBox_Category->addItem(QString("%1 [%2]").arg(i.value()).arg(i.key()), QVariant(i.key()));

    m_ui->comboBox_Priority->clear();
    for (int i = 0; i < Ticket::Priorities.length(); i++)
        m_ui->comboBox_Priority->addItem(QString::number(Ticket::Priorities.at(i)));

    m_ui->comboBox_Status->clear();
    for (QMap<QString, QString>::const_iterator i = Ticket::Statuses.constBegin(); i != Ticket::Statuses.constEnd(); i++)
        m_ui->comboBox_Status->insertItem(0, i.value(), i.key());

    m_ui->comboBox_Group->clear();
    for (QMap<int, QString>::const_iterator i = Ticket::Groups.constBegin(); i != Ticket::Groups.constEnd(); ++i)
        if (i.key() > 0)
            m_ui->comboBox_Group->addItem(QString("%1 [%2]").arg(i.value()).arg(i.key()), QVariant(i.key()));
}

void TicketWidget::on_pushButton_Update_clicked()
{
    if (m_ui->comboBox_Hostopian->currentIndex() == -1)
        QMessageBox::information(this, tr("Alert"), tr("Please specify Hostopian!"));
    else if (m_ui->comboBox_Category->currentIndex() == -1)
        QMessageBox::information(this, tr("Alert"), tr("Please specify Category!"));
    else if (m_ui->comboBox_SubCategory->currentIndex() == -1)
        QMessageBox::information(this, tr("Alert"), tr("Please specify SubCategory!"));
    else if (m_ui->comboBox_Priority->currentIndex() == -1)
        QMessageBox::information(this, tr("Alert"), tr("Please specify Priority!"));
    else if (m_ui->comboBox_Status->currentIndex() == -1)
        QMessageBox::information(this, tr("Alert"), tr("Please specify Status!"));
    else if (m_ui->comboBox_Group->currentIndex() == -1)
        QMessageBox::information(this, tr("Alert"), tr("Please specify Assigned Group!"));
    else if (m_ui->comboBox_Person->currentIndex() == -1)
        QMessageBox::information(this, tr("Alert"), tr("Please specify Assigned Tech!"));
    else if (QMessageBox::information(this, tr("Alert"), tr("Proceed?"), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
    {
        ticket.Hostopian = m_ui->comboBox_Hostopian->currentText();
        ticket.Domain = m_ui->lineEdit_Domain->text();
        ticket.Name = m_ui->lineEdit_CustName->text();
        ticket.Email = m_ui->lineEdit_Email->text();
        ticket.Problem = m_ui->plainTextEdit_Problem->toPlainText();
        ticket.Notes = m_ui->plainTextEdit_Notes->toPlainText();
        ticket.Resolution = m_ui->plainTextEdit_Resolution->toPlainText();
        ticket.AuditTrail = m_ui->plainTextEdit_AuditTrail->toPlainText();
        ticket.Category = m_ui->comboBox_Category->itemData(m_ui->comboBox_Category->currentIndex()).toInt();
        ticket.SubCategory = m_ui->comboBox_SubCategory->currentText();
        ticket.Priority = m_ui->comboBox_Priority->currentText().toInt();
        ticket.setStatus(m_ui->comboBox_Status->itemData(m_ui->comboBox_Status->currentIndex()).toString());
        ticket.AssignedGroup = m_ui->comboBox_Group->itemData(m_ui->comboBox_Group->currentIndex()).toInt();
        ticket.AssignedTech.clear();
        ticket.AssignedTech.insert(QString(), m_ui->comboBox_Person->itemData(m_ui->comboBox_Person->currentIndex()).toString());

        m_ui->pushButton_Update->setEnabled(false);
        emit updateTicket(ticket);
    }
}

void TicketWidget::changeEvent(QEvent *e)
{
    switch(e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        if (ticket.CaseID > 0) {
            m_ui->label_CaseID->setText(QString("<a style=\"text-decoration: underline; color:#0000ff;\" href=\"https://support.24hourwebhostingsupport.com/showcases.php?showme=%1\">%2</a>").arg(ticket.CaseID).arg(tr("CaseID")));
        }
        break;
    default:
        break;
    }
}

void TicketWidget::on_comboBox_Category_currentIndexChanged(int index)
{
    QComboBox * box = m_ui->comboBox_SubCategory;
    box->clear();
    if (index >= 0)
    {
        int i = m_ui->comboBox_Category->itemData(index, Qt::UserRole).toInt();
        if (Ticket::SubCategories.contains(i))
            box->addItems(Ticket::SubCategories.value(i));
        box->setCurrentIndex(-1);
    }
}

void TicketWidget::on_comboBox_Group_currentIndexChanged(int index)
{
    QComboBox * box = m_ui->comboBox_Person;
    box->clear();
    if (index != -1)
    {
        int id = m_ui->comboBox_Group->itemData(index, Qt::UserRole).toInt();
        if (id ==  ticket.AssignedGroup)
        {
            for (QMap<QString, QString>::const_iterator i = ticket.AssignedTech.constBegin(); i != ticket.AssignedTech.constEnd(); ++i)
                if (!i.key().isNull())
                    box->addItem(QString("%1 [%2]").arg(i.value()).arg(i.key()), QVariant(i.key()));
            box->setCurrentIndex(box->findData(QVariant(ticket.AssignedTech.value(QString()))));
        } else
        {
            if (Ticket::GlobalTechs.contains(id))
            {
                QMapIterator<QString, QString> iter(Ticket::GlobalTechs.value(id));
                while (iter.hasNext())
                {
                    iter.next();
                    box->addItem(QString("%1 [%2]").arg(iter.value()).arg(iter.key()), QVariant(iter.key()));
                }
            } else {
                QString val(Ticket::GlobalTechs.value(-1).values().at(0));
                QString key(Ticket::GlobalTechs.value(-1).keys().at(0));
                box->addItem(QString("%1 [%2]").arg(val).arg(key), QVariant(key));
            }
        }
    }
}

void TicketWidget::on_pushButton_Reset_clicked()
{
    updateContent(ticket);
}

void TicketWidget::on_toolButton_clicked()
{
    emit getDomainInfo(m_ui->lineEdit_Domain->text());
}

TicketWidget::~TicketWidget()
{
    delete m_ui;
}

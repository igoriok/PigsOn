#include "Ticket.h"

QStringList Ticket::Hostopians;
QMap<int, QString> Ticket::Categories;
QHash<int, QStringList> Ticket::SubCategories;
QList<int> Ticket::Priorities;
QMap<QString, QString> Ticket::Statuses;
QMap<int, QString> Ticket::Groups;
QHash<int, QStringMap> Ticket::GlobalTechs;

Ticket::Ticket()
{
    CaseID = 0;
}

bool Ticket::isValid() const
{
    return (CaseID != 0);
}

void Ticket::setStatus(const QString & status)
{
    if (status != this->Status)
    {
        this->StatusOld = this->StatusHidden;
        this->StatusHidden = status;
        this->Status = status;
        this->StatusChange = QString("yes");
    }
}


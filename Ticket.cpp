#include "Ticket.h"

QStringList Ticket::Hostopians;
QMap<int, QString> Ticket::Categories;
QHash<int, QStringList> Ticket::SubCategories;
QMap<int, QString> Ticket::Groups;
QHash<int, QStringMap> Ticket::GlobalTechs;

Ticket::Ticket()
{
    CaseID = 0;
}

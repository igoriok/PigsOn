#include "TicketInfo.h"
#include <QtCore/QRegExp>
#include <QtCore/QLocale>

TicketInfo::TicketInfo()
{
}

TicketInfo::TicketInfo(
            QUrl url,
            QString caseID,
            QString category,
            QString hostopian,
            QString name,
            QString domain,
            QString status,
            int priority,
            QString opened
) :
        m_Url(url),
        m_CaseID(caseID),
        m_Category(category),
        m_Hostopian(hostopian),
        m_Name(name),
        m_Domain(domain),
        m_Status(status),
        m_Priority(priority),
        m_Opened(opened)
{
}

QString TicketInfo::toString() const
{
    return m_CaseID;
}

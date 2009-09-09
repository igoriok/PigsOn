#include "TicketInfo.h"
#include <QtCore/QRegExp>
#include <QtCore/QLocale>

TicketInfo::TicketInfo(
            const QUrl & url,
            const QString & caseID,
            const QString & category,
            const QString & hostopian,
            const QString & name,
            const QString & domain,
            const QString & status,
            int priority,
            const QString & opened
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


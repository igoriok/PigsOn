#ifndef TICKETINFO_H
#define TICKETINFO_H

#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QUrl>

class TicketInfo
{
public:
    TicketInfo();
    TicketInfo(
            QUrl url,
            QString caseID,
            QString category,
            QString hostopian,
            QString name,
            QString domain,
            QString status,
            int priority,
            QString opened
    );
    // Getters
    QUrl getUrl() const {return m_Url;}
    QString getCaseID() const {return m_CaseID;}
    QString getCategory() const {return m_Category;}
    QString getHostopian() const {return m_Hostopian;}
    QString getName() const {return m_Name;}
    QString getDomain() const {return m_Domain;}
    QString getStatus() const {return m_Status;}
    int getPriority() const {return m_Priority;}
    QString getOpened() const {return m_Opened;}

public:
    QString toString() const;

private:
    QUrl m_Url;
    QString m_CaseID;
    QString m_Category;
    QString m_Hostopian;
    QString m_Name;
    QString m_Domain;
    QString m_Status;
    int m_Priority;
    QString m_Opened;
};

#endif // TICKETINFO_H

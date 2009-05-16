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
    TicketInfo(
            const QUrl & url,
            const QString & caseID,
            const QString & category,
            const QString & hostopian,
            const QString & name,
            const QString & domain,
            const QString & status,
            int priority,
            const QString & opened
    );
    // Getters
    const QUrl & getUrl() const {return m_Url;}
    const QString & getCaseID() const {return m_CaseID;}
    const QString & getCategory() const {return m_Category;}
    const QString & getHostopian() const {return m_Hostopian;}
    const QString & getName() const {return m_Name;}
    const QString & getDomain() const {return m_Domain;}
    const QString & getStatus() const {return m_Status;}
    int getPriority() const {return m_Priority;}
    const QString & getOpened() const {return m_Opened;}

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

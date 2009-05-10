#ifndef PIGSCLIENT_H
#define PIGSCLIENT_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include "SAccount.h"
#include "Ticket.h"
#include "TicketInfo.h"

// Abstract class for different types of Pigs client: html, xml, db.
class PigsClient : public QObject
{
    Q_OBJECT
public:
    PigsClient(QObject * parent) : QObject(parent), debug(false) {}
    virtual ~PigsClient() {}

    virtual void setAccount(Account acc) =0;
    bool debug;

    enum PigsRequest
    {
        Unknown,
        RefreshGlobals,
        GetGroupTickets,
        SearchTickets,
        GetTicket,
        GetDomainInfo,
        CreateTicket,
        UpdateTicket
    };

public slots:
    virtual void refreshGlobals() =0;
    virtual void getGroupTickets(int groupID) =0;
    virtual void searchTickets(const QMap<QString, QString> & data) = 0;
    virtual void getTicket(int caseID) =0;
    virtual void getDomainInfo(const QString & domain) =0;
    virtual void createTicket(const Ticket & ticket) =0;
    virtual void updateTicket(const Ticket & ticket) =0;

signals:
    void globalsReady();
    void groupTicketsReady(int id, const QList<TicketInfo> & tickets);
    void searchReady(const QList<TicketInfo> & tickets);
    void ticketReady(const Ticket & ticket);
    void domainInfoReady(const QString & info);
    void error(QString error, PigsRequest req, int id);
    void showMessage(QString mess);
};

#endif // PIGSCLIENT_H

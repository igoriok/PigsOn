#ifndef PIGSCLIENT_H
#define PIGSCLIENT_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include "SAccount.h"
#include "Ticket.h"
#include "TicketInfo.h"

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

// Abstract class for different types of Pigs client: html, xml, db.
class GenericPigsClient : public QObject
{
    Q_OBJECT
public:
    GenericPigsClient(QObject * parent) : QObject(parent), debug(false) {}
    virtual ~GenericPigsClient() {}

    virtual void setAccount(Account acc) =0;
    bool debug;

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

// HTML Pigs client
class PigsClient : public GenericPigsClient
{
    Q_OBJECT
public:
    PigsClient(QObject * parent = 0);
    ~PigsClient() {}

    // Declare parent abstract functions
    virtual void setAccount(Account acc);

    virtual void refreshGlobals();
    virtual void getGroupTickets(int groupID);
    virtual void searchTickets(const QMap<QString, QString> & data);
    virtual void getTicket(int caseID);
    virtual void getDomainInfo(const QString & domain);
    virtual void createTicket(const Ticket & ticket);
    virtual void updateTicket(const Ticket & ticket);

private:
    QNetworkAccessManager * client;
    Account account;

    Ticket parseTicket(const QString & data);
    QString subString(QString start, QString end, int & pos, const QString & data);
    QString fromHtml(const QString & data);
    void parseHostopians(const QString & data);
    void parseCategories(const QString & data);
    void parseSubCategories(const QString & data);
    void parseGlobalTechs(const QString & data);
    void parseGroups(const QString & data);
    QMap<QString, QString> parseSelect(const QString & data);
    QList<TicketInfo> parseTicketInfo(const QString & data);
    QByteArray prepareQuery(const Ticket & ticket);
    QByteArray toPostText(const QString & text);

private slots:
    void on_client_finished(QNetworkReply * reply);
    void on_client_sslErrors(QNetworkReply * reply);
};

#endif // PIGSCLIENT_H

#ifndef HTMLPIGSCLIENT_H
#define HTMLPIGSCLIENT_H

#include <QtNetwork/QNetworkReply>
#include "PigsClient.h"
#include "SAccount.h"
#include "Ticket.h"
#include "TicketInfo.h"

// HTML Pigs client
class HtmlPigsClient : public PigsClient
{
    Q_OBJECT
public:
    HtmlPigsClient(QObject * parent = 0);
    ~HtmlPigsClient() {}

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

#endif // HTMLPIGSCLIENT_H

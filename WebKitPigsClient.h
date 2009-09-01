#ifndef WEBKITPIGSCLIENT_H
#define WEBKITPIGSCLIENT_H

#include <QtNetwork/QNetworkReply>
#include <QWebPage>
#include "PigsClient.h"
#include "SAccount.h"
#include "Ticket.h"
#include "TicketInfo.h"
#include <QQueue>

// HTML Pigs client
class WebKitPigsClient : public PigsClient
{
    Q_OBJECT
public:
    WebKitPigsClient(QObject * parent = 0);
    ~WebKitPigsClient() {}

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
    struct QueryRequest
    {
        QueryRequest(PigsRequest requestType,
                     const QNetworkRequest & request,
                     QNetworkAccessManager::Operation operation = QNetworkAccessManager::GetOperation,
                     const QByteArray & data = QByteArray(),
                     int requestId = 0)
        {
            this->request = request;
            this->operation = operation;
            this->requestType = requestType;
            this->data = data;
            this->requestId = requestId;
        }
        PigsRequest requestType;
        QNetworkRequest request;
        QNetworkAccessManager::Operation operation;
        QByteArray data;
        int requestId;
    };

    QWebPage * page;
    Account account;
    QQueue<QueryRequest> requestQueue;

    void addToQuery(const QueryRequest & req);
    void nextQueryItem();

private slots:
    void on_page_manager_sslErrors(QNetworkReply * reply);
    void on_page_unsupportedContent(QNetworkReply * reply);
    void on_page_loadFinished(bool b);
};

#endif // WEBKITPIGSCLIENT_H

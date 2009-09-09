#ifndef WEBKITPIGSCLIENT_H
#define WEBKITPIGSCLIENT_H

#include <QtNetwork/QNetworkReply>
#include <QWebPage>
#include <QQueue>
#include "PigsClient.h"
#include "SAccount.h"
#include "Ticket.h"
#include "TicketInfo.h"

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
                     int requestId,
                     const QNetworkRequest & request,
                     QNetworkAccessManager::Operation operation = QNetworkAccessManager::GetOperation,
                     const QByteArray & data = QByteArray())
        {
            this->request = request;
            this->requestId = requestId;
            this->operation = operation;
            this->requestType = requestType;
            this->data = data;
        }

        PigsRequest requestType;
        int requestId;
        QNetworkRequest request;
        QNetworkAccessManager::Operation operation;
        QByteArray data;
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

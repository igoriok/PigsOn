#include "WebKitPigsClient.h"
#include "WebKitPigsParser.h"
#include <QtCore/QRegExp>
#include <QtCore/QLocale>
#include <QtNetwork/QNetworkCookieJar>
#include <QtNetwork/QSslError>
#include <QtGui/QMessageBox>
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QWebFrame>
#include <QMessageBox>

WebKitPigsClient::WebKitPigsClient(QObject * parent): PigsClient(parent)
{
    page = new QWebPage(this);
    page->setForwardUnsupportedContent(true);
    page->settings()->setAttribute(QWebSettings::AutoLoadImages, false);
    page->settings()->setAttribute(QWebSettings::PrintElementBackgrounds, false);

    this->connect(page->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply *, QList<QSslError>)), SLOT(on_page_manager_sslErrors(QNetworkReply *)));
    this->connect(page, SIGNAL(unsupportedContent(QNetworkReply*)), SLOT(on_page_unsupportedContent(QNetworkReply*)));
    this->connect(page, SIGNAL(loadFinished(bool)), SLOT(on_page_loadFinished(bool)));
}

void WebKitPigsClient::setAccount(Account acc)
{
    account = acc;

    if (account.isValid())
    {
        QList<QNetworkCookie> cookies;
        QByteArray data;
        data.append(account.user);
        data.append("%3A");
        data.append(account.pass);
        cookies.append(QNetworkCookie("pigslog", data));
        cookies.append(QNetworkCookie("pigstatus", data));
        page->networkAccessManager()->cookieJar()->setCookiesFromUrl(cookies, QUrl("https://support.24hourwebhostingsupport.com/"));

        //refreshGlobals();
    }
    else
    {
        page->networkAccessManager()->setCookieJar(new QNetworkCookieJar());

        Ticket::Hostopians.clear();
        Ticket::Categories.clear();
        Ticket::SubCategories.clear();
        Ticket::GlobalTechs.clear();
        Ticket::Groups.clear();
    }
}

void WebKitPigsClient::addToQuery(const QueryRequest & req)
{
    requestQueue.enqueue(req);

    if (requestQueue.length() == 1)
        nextQueryItem();
}

void WebKitPigsClient::nextQueryItem()
{
    if (!requestQueue.isEmpty())
    {
        QueryRequest req = requestQueue.head();
        switch (req.requestType)
        {
            case Unknown:
                break;
            case RefreshGlobals:
                emit showMessage(tr("Retrieve Globals..."));
                break;
            case GetGroupTickets:
                emit showMessage(tr("Retrieve %1 tickets...").arg(Ticket::Groups.value(req.requestId)));
                break;
            case SearchTickets:
                emit showMessage(tr("Searching..."));
                break;
            case GetTicket:
                emit showMessage(tr("Retrieve ticket %1...").arg(req.requestId));
                break;
            case GetDomainInfo:
                emit showMessage(tr("Getting domain info..."));
                break;
            case CreateTicket:
                emit showMessage(tr("Creating ticket..."));
                break;
            case UpdateTicket:
                emit showMessage(tr("Updating ticket %1...").arg(req.requestId));
                break;
            default:
                break;
        }

        page->mainFrame()->load(req.request, req.operation, req.data);
    }
}

void WebKitPigsClient::refreshGlobals()
{
    addToQuery(QueryRequest(RefreshGlobals, 0,
                            QNetworkRequest(QUrl("https://support.24hourwebhostingsupport.com/showcases.php"))));
}

void WebKitPigsClient::getGroupTickets(int groupID)
{
    if (groupID < 0)
    {
        emit error(tr("Please provide correct Group ID"), GetGroupTickets, groupID);
        return;
    }

    QUrl url("https://support.24hourwebhostingsupport.com/cases.php");
    QList<QPair<QString, QString> > query;
    query.append(QPair<QString, QString>("request", "mysearch"));

    if (groupID == 0)
        // Setup url for user's tickets
        query.append(QPair<QString, QString>("cookiesql", QString("select+*+from+CASES+where+atech='%1'+and+status+in+('in_progress','open')+order+by+priority").arg(account.user)));
    else
        // Setup url for user's group tickets
        query.append(QPair<QString, QString>("cookiesql", QString("select+*+from+CASES+where+agroup=%1+and+atech='group'+and+status+in+('in_progress','open')+order+by+priority").arg(groupID)));
    url.setQueryItems(query);

    addToQuery(QueryRequest(GetGroupTickets, groupID, QNetworkRequest(url)));
}

void WebKitPigsClient::searchTickets(const QMap<QString, QString> & data)
{
    QByteArray postData;

    for (QMap<QString, QString>::const_iterator iter = data.constBegin(); iter != data.constEnd(); ++iter)
        postData.append(QByteArray().append(iter.key()).append("=").append(WebKitPigsParser::toPostText(iter.value())).append("&"));
    if (data.contains(QString("statussearch")))
        postData.append("and=entered&");
    else
        postData.append("statussearch=open&");
    postData.append("request=search&findit=set&submit=search");

    addToQuery(QueryRequest(SearchTickets, 0,
                            QNetworkRequest(QUrl(
                                    QString("https://support.24hourwebhostingsupport.com/cases.php?request=search"))),
                            QNetworkAccessManager::PostOperation,
                            postData));
}

void WebKitPigsClient::getTicket(int caseID)
{
    if (caseID <= 0)
    {
        emit error(tr("Please provide correct Case ID"), GetTicket, caseID);
        return;
    }

    addToQuery(QueryRequest(GetTicket,
                            caseID,
                            QNetworkRequest(QUrl(
                                    QString("https://support.24hourwebhostingsupport.com/showcases.php?showme=%1")
                                    .arg(caseID)))
                            ));
}

void WebKitPigsClient::getDomainInfo(const QString & domain)
{
    if (domain.isEmpty())
    {
        emit error(tr("Please provide correct domain name"), GetDomainInfo, 0);
        return;
    }

    QByteArray postData("thisdomain=");
    postData.append(WebKitPigsParser::toPostText(domain));

    addToQuery(QueryRequest(GetDomainInfo, 0,
                            QNetworkRequest(QUrl(
                                    QString("https://support.24hourwebhostingsupport.com/cases.php?request=fhostopian"))),
                            QNetworkAccessManager::PostOperation,
                            postData));
}

void WebKitPigsClient::createTicket(const Ticket & ticket)
{
}

void WebKitPigsClient::updateTicket(const Ticket & ticket)
{
    QByteArray data(WebKitPigsParser::prepareQuery(ticket));
    if (debug)
    {
        QFile f(QString("./PigsON.log"));
        if (f.open(QIODevice::Append))
        {
            f.write(QByteArray(data).replace('&', '\n').append("\n\n"));
            f.close();
        }
    }

    addToQuery(QueryRequest(UpdateTicket, ticket.CaseID,
                            QNetworkRequest(QUrl(QString("https://support.24hourwebhostingsupport.com/showcases.php"))),
                            QNetworkAccessManager::PostOperation,
                            data));
}

void WebKitPigsClient::on_page_manager_sslErrors(QNetworkReply * reply)
{
    reply->ignoreSslErrors();
}

void WebKitPigsClient::on_page_unsupportedContent(QNetworkReply * reply)
{
    QueryRequest req = requestQueue.head();
    emit error(reply->errorString(), req.requestType, req.requestId);
}

void WebKitPigsClient::on_page_loadFinished(bool b)
{
    QueryRequest req = requestQueue.head();

    if(b) // if not connection error
    {

        if(WebKitPigsParser::isAccessDeny(page->mainFrame())) // if login not accepted
            emit error(tr("Login or password not accepted!"), req.requestType, req.requestId);
        else
            switch(req.requestType)
            {
                case RefreshGlobals:
                    WebKitPigsParser::parseHostopians(page->mainFrame());
                    WebKitPigsParser::parseCategories(page->mainFrame());
                    WebKitPigsParser::parseSubCategories(page->mainFrame());
                    WebKitPigsParser::parsePriorities(page->mainFrame());
                    WebKitPigsParser::parseStatuses(page->mainFrame());
                    WebKitPigsParser::parseGlobalTechs(page->mainFrame());
                    WebKitPigsParser::parseGroups(page->mainFrame());
                    emit globalsReady();
                    break;
                case GetGroupTickets:
                    emit groupTicketsReady(req.requestId, WebKitPigsParser::parseTicketInfo(page->mainFrame(), req.requestType));
                    break;
                case SearchTickets:
                    emit searchReady(WebKitPigsParser::parseTicketInfo(page->mainFrame(), req.requestType));
                    break;
                case GetTicket:
                    {
                        Ticket t = WebKitPigsParser::parseTicket(page->mainFrame());
                        if (t.CaseID == req.requestId)
                            emit ticketReady(t);
                        else
                            emit error(tr("Returned Case ID is incorrect: %1").arg(t.CaseID), req.requestType, req.requestId);
                    }
                    break;
                case GetDomainInfo:
                    emit domainInfoReady(WebKitPigsParser::parseDomainInfo(page->mainFrame()));
                    break;
                case CreateTicket:
                    break;
                case UpdateTicket:
                    getTicket(req.requestId);
                    break;
                default:
                    emit error(tr("Unknown request"), req.requestType, req.requestId);
                    break;
            }
    }

    requestQueue.dequeue();
    nextQueryItem();
}

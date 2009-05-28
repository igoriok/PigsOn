#include "HtmlPigsClient.h"
#include <QtCore/QRegExp>
#include <QtCore/QLocale>
#include <QtNetwork/QNetworkCookieJar>
#include <QtNetwork/QSslError>
#include <QtGui/QMessageBox>
#include <QtCore/QFile>
#include <QtCore/QTextCodec>

HtmlPigsClient::HtmlPigsClient(QObject * parent): PigsClient(parent)
{
    client = new QNetworkAccessManager(this);
    client->setObjectName("client");

    this->connect(client, SIGNAL(sslErrors(QNetworkReply *, QList<QSslError>)), SLOT(on_client_sslErrors(QNetworkReply *)));
    this->connect(client, SIGNAL(finished(QNetworkReply *)), this, SLOT(on_client_finished(QNetworkReply *)));
}

void HtmlPigsClient::setAccount(Account acc)
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
        client->cookieJar()->setCookiesFromUrl(cookies, QUrl("https://support.24hourwebhostingsupport.com/"));

        refreshGlobals();
    }
    else
    {
        client->setCookieJar(new QNetworkCookieJar());

        Ticket::Hostopians.clear();
        Ticket::Categories.clear();
        Ticket::SubCategories.clear();
        Ticket::GlobalTechs.clear();
        Ticket::Groups.clear();
    }
}

void HtmlPigsClient::refreshGlobals()
{
    emit showMessage(tr("Retrieve Globals..."));
    QNetworkRequest req(QUrl("https://support.24hourwebhostingsupport.com/showcases.php"));
    req.setAttribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 1), QVariant((int)RefreshGlobals));
    req.setAttribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 2), QVariant(int(0)));
    client->get(req);
}

void HtmlPigsClient::getGroupTickets(int groupID)
{
    emit showMessage(tr("Retrieve %1 tickets...").arg(Ticket::Groups.value(groupID)));
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

    QNetworkRequest req(url);
    req.setAttribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 1), QVariant((int)GetGroupTickets));
    req.setAttribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 2), QVariant(groupID));
    client->get(req);
}

void HtmlPigsClient::searchTickets(const QMap<QString, QString> & data)
{
    emit showMessage(tr("Searching..."));
    QNetworkRequest req(QUrl(QString("https://support.24hourwebhostingsupport.com/cases.php?request=search")));
    QByteArray postData;

    req.setAttribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 1), QVariant((int)SearchTickets));
    req.setAttribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 2), QVariant(int(0)));

    for (QMap<QString, QString>::const_iterator iter = data.constBegin(); iter != data.constEnd(); ++iter)
        postData.append(QByteArray().append(iter.key()).append("=").append(toPostText(iter.value())).append("&"));
    if (data.contains(QString("statussearch")))
        postData.append("and=entered&");
    else
        postData.append("statussearch=open&");
    postData.append("request=search&findit=set&submit=search");

    client->post(req, postData);
}

void HtmlPigsClient::getTicket(int caseID)
{
    emit showMessage(tr("Retrieve ticket %1...").arg(caseID));
    if (caseID <= 0)
    {
        emit error(tr("Please provide correct Case ID"), GetTicket, caseID);
        return;
    }

    QNetworkRequest req(QUrl(QString("https://support.24hourwebhostingsupport.com/showcases.php?showme=%1").arg(caseID)));
    req.setAttribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 1), QVariant((int)GetTicket));
    req.setAttribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 2), QVariant(caseID));
    client->get(req);
}

void HtmlPigsClient::getDomainInfo(const QString & domain)
{
    emit showMessage(tr("Getting domain info %1...").arg(domain));
    if (domain.isEmpty())
    {
        emit error(tr("Please provide correct domain name"), GetDomainInfo, 0);
        return;
    }

    QNetworkRequest req(QUrl(QString("https://support.24hourwebhostingsupport.com/cases.php?request=fhostopian")));
    QByteArray postData("thisdomain=");
    postData.append(toPostText(domain));

    req.setAttribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 1), QVariant((int)GetDomainInfo));
    req.setAttribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 2), QVariant(int(0)));

    client->post(req, postData);
}

void HtmlPigsClient::createTicket(const Ticket & ticket)
{
}

void HtmlPigsClient::updateTicket(const Ticket & ticket)
{
    emit showMessage(tr("Updating ticket %1...").arg(ticket.CaseID));
        QByteArray data(prepareQuery(ticket));
    if (debug)
    {
        QFile f(QString("./PigsON.log"));
        if (f.open(QIODevice::Append))
        {
            f.write(QByteArray(data).replace('&', '\n').append("\n\n"));
            f.close();
        }
    }
    QNetworkRequest req(QUrl(QString("https://support.24hourwebhostingsupport.com/showcases.php")));
    req.setAttribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 1), QVariant((int)UpdateTicket));
    req.setAttribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 2), QVariant(ticket.CaseID));
    client->post(req, data);
}

Ticket HtmlPigsClient::parseTicket(const QString & data)
{
    Ticket ticket;
    int pos = 0;
    ticket.CaseID = subString("<td align=left valign=top>&nbsp;&nbsp;&nbsp;<b>CaseID:</b>&nbsp;", "</td>", pos, data).toInt();
    if (ticket.CaseID > 0)
    {
        ticket.DateOpened = subString("<td align=left valign=top>&nbsp;&nbsp;&nbsp;<b>Date opened:</b>&nbsp;", "</td>", pos, data);
        ticket.DateClosed = subString("<td align=left valign=top><b>Date closed:</b>&nbsp;", "</td>", pos, data);
        ticket.Hostopian = parseSelect(subString("<select name=nchostopian>", "</select>", pos, data)).value(QString());
        ticket.HostopianHidden = subString("<input type=hidden name=hiddenhostopian value=\"", "\">", pos, data);
        ticket.Domain = subString("<input type=text name=ncdomain value=\"", "\">", pos, data);
        ticket.Name = subString("<input type=text name=ncname value=\"", "\">", pos, data);
        ticket.Email = subString("<input type=text name=ncemail value=\"", "\">", pos, data);
        ticket.CategoryHidden = subString("<input type=\"hidden\" name=\"ncategory\" value=\"", "\">", pos, data);
        ticket.SubCategoryHidden = subString("<input type=\"hidden\" name=\"nsubcategory\" value=\"", "\">", pos, data);
        ticket.Category = parseSelect(subString("<select name=\"category1\"", "</select>", pos, data)).value(QString()).toInt();
        ticket.SubCategory = parseSelect(subString("<select name=\"subcategory1\"", "</select>", pos, data)).value(QString());
        ticket.Problem = fromHtml(subString("<textarea name=ncproblem cols=60 rows=6 wrap=physical>", "</textarea>", pos, data));
        ticket.ProblemHidden = subString("<input type=hidden name=hiddenproblem value=\"", "\">", pos, data);
        ticket.Priority = parseSelect(subString("<select name=ncpriority>", "</select>", pos, data)).value(QString()).toInt();
        ticket.PriorityHidden = subString("<input type=hidden name=hiddenpriority value=", ">", pos, data);
        ticket.Status = parseSelect(subString("<select name=ncstatus onchange=changestatus()>", "</select>", pos, data)).value(QString());
        ticket.StatusHidden = subString("<input type=hidden name=hiddenstatus value=", ">", pos, data);
        ticket.StatusOld = subString("<input type=hidden name=oldstatus value=", ">", pos, data);
        ticket.Notes = fromHtml(subString("<textarea name=ncnotes cols=60 rows=6 wrap=physical>", "</textarea>", pos, data));
        ticket.NotesHidden = subString("<input type=hidden name=hiddennotes value=\"", "\">", pos, data);
        ticket.Resolution = fromHtml(subString("<textarea name=ncresolution cols=60 rows=6 wrap=physical>", "</textarea>", pos, data));
        ticket.OpenedByTech = subString("<input type=hidden name=ncotech value=\"", "\">", pos, data);
        ticket.AssignedGroup = parseSelect(subString("<select name=ncagroup", "</select>", pos, data)).value(QString()).toInt();
        ticket.TechName = subString("<input type=hidden name=techname value=\"", "\">", pos, data);
        ticket.TechEmail = subString("<input type=hidden name=techemail value=\"", "\">", pos, data);
        ticket.ClosedByTech = subString("<b>Closed by:</b> &nbsp;", "</td>", pos, data);
        ticket.AssignedTech = parseSelect(subString("<select name=ncatech>", "</select>", pos, data));
        ticket.AssignedTechHidden = subString("<input type=hidden name=hiddenatech value=\"", "\">", pos, data);
        ticket.AuditTrail = subString("<textarea name=uaudit cols=60 rows=6 wrap=physical>", "</textarea>", pos, data);
        ticket.StatusChange = subString("<input type=hidden name=stchange value=\"", "\">", pos, data);
        ticket.AuditHidden = subString("<input type=hidden name=ncaudit value=\"", "\">", pos, data);
        ticket.AuditPrev = subString("<table width=95% border=0 cellpadding=0 cellspacing=0>", "</table>", pos, data);
    }
    return ticket;
}

QString HtmlPigsClient::subString(QString start, QString end, int & pos, const QString & data)
{
    int spos = data.indexOf(start, pos);
    if (spos != -1)
    {
        spos += start.size();
        int epos = data.indexOf(end, spos);
        if (epos != -1)
        {
            pos = epos + end.size();
            return data.mid(spos, epos - spos);
        }
    }
    return QString();
}

QString HtmlPigsClient::fromHtml(const QString & data)
{
    return QString(data).replace(QString("&lt;"), QString("<")).replace(QString("&gt;"), QString(">")).replace(QString("&amp;"), QString("&")).replace(QString("&apos;"), QString("'")).replace(QString("&quot;"), QString("\""));
}

void HtmlPigsClient::parseHostopians(const QString & data)
{
    QRegExp reg("<select name=nchostopian>[\\s]*((?:<option value='[^']*'>[^<]*</option>)+)[\\s]*</select>");
    if (reg.indexIn(data) != -1)
    {
        Ticket::Hostopians.clear();

        QString data1(reg.cap(1));
        QRegExp reg1("<option value='([^']+)'>[^<]*</option>");
        int pos1 = 0;

        while ((pos1 = reg1.indexIn(data1, pos1)) != -1)
        {
            Ticket::Hostopians.append(reg1.cap(1));
            pos1 += reg1.matchedLength();
        }
        Ticket::Hostopians.sort();
    }
}

void HtmlPigsClient::parseCategories(const QString & data)
{
    QRegExp reg("<select name=\"category1\"[^>]*>>[\\s]*((?:<OPTION value=\"[\\d]+\"(?: SELECTED)?>[^<]*</OPTION>)+);[\\s]*</select>");
    if (reg.indexIn(data) != -1)
    {
        Ticket::Categories.clear();

        QString data1(reg.cap(1));
        QRegExp reg1("<OPTION value=\"([\\d]+)\"(?: SELECTED)?>([^<]*)</OPTION>");
        int pos1 = 0;
        while ((pos1 = reg1.indexIn(data1, pos1)) != -1)
        {
            Ticket::Categories.insert(reg1.cap(1).toInt(), reg1.cap(2));
            pos1 += reg1.matchedLength();
        }
    }
}

void HtmlPigsClient::parseSubCategories(const QString & data)
{
    QRegExp reg("function setOptions\\(chosen\\) \\{[\\w\\s\\.;=]*((?:if \\(chosen == [\"'][^'\"]*[\"']\\)[\\s]*\\{[\\s\\w\\d/]*(?:selbox\\.options\\[selbox\\.options\\.length\\] = new Option\\('[^']*','[^']*'\\);[\\s]*)+\\}[\\s]*)+)\\}");
    if (reg.indexIn(data) != -1)
    {
        Ticket::SubCategories.clear();

        QString data1(reg.cap(1));
        QRegExp reg1("if \\(chosen == [\"']([^'\"]*)[\"']\\)[\\s]*\\{[\\s\\w\\d/]*((?:selbox\\.options\\[selbox\\.options\\.length\\] = new Option\\('[^']*','[^']*'\\);[\\s]*)+)\\}");
        int pos1 = 0;
        while ((pos1 = reg1.indexIn(data1, pos1)) != -1)
        {
            if (!reg1.cap(1).isEmpty())
            {
                int id = reg1.cap(1).toInt();
                if (id > 0)
                {
                    QStringList list;
                    QString data2(reg1.cap(2));

                    QRegExp reg2("selbox\\.options\\[selbox\\.options\\.length\\] = new Option\\('([^']*)','[^']*'\\);");
                    int pos2 = 0;
                    while ((pos2 = reg2.indexIn(data2, pos2)) != -1)
                    {
                        list<<reg2.cap(1);
                        pos2 += reg2.matchedLength();
                    }
                    if (!list.isEmpty())
                        Ticket::SubCategories.insert(id, list);
                }
            }
            pos1 += reg1.matchedLength();
        }
    }
}

void HtmlPigsClient::parseGlobalTechs(const QString & data)
{
    QRegExp reg("function setOptions2\\(chosen\\)[\\s]*\\{[\\w\\s\\.;=]*if \\(chosen == \"\"\\)[\\s]*\\{[\\s]*(?:selbox\\.options\\[selbox\\.options\\.length\\] = new Option\\('[^']*','[^']*'\\);[\\s]*)+\\}[\\s]*if \\(chosen == '600'\\)[\\s]*\\{[\\s]*((?:selbox\\.options\\[selbox\\.options\\.length\\] = new Option\\('[^']*','[^']*'\\);[\\s]*)+)\\}[\\s]*else if \\( '405' == chosen\\)[\\s]*\\{[\\s]*((?:selbox\\.options\\[selbox\\.options\\.length\\] = new Option\\('[^']*','[^']*'\\);[\\s]*)+)\\}[\\s]*else[\\s]*\\{[\\s]*((?:selbox\\.options\\[selbox\\.options\\.length\\] = new Option\\('[^']*','[^']*'\\);[\\s]*)+)\\}[\\s]*\\}");
    if (reg.indexIn(data) != -1)
    {
        Ticket::GlobalTechs.clear();

        QRegExp reg1("selbox\\.options\\[selbox\\.options\\.length\\] = new Option\\('([^']*)','([^']*)'\\);");
        QMap<QString, QString> hash;
        QString data1(reg.cap(1));
        int pos1 = 0;
        while ((pos1 = reg1.indexIn(data1, pos1)) != -1)
        {
            hash.insert(reg1.cap(2), reg1.cap(1));
            pos1 += reg1.matchedLength();
        }
        if (!hash.isEmpty())
            Ticket::GlobalTechs.insert(600, hash);

        hash.clear();
        data1 = reg.cap(2);
        pos1 = 0;
        while ((pos1 = reg1.indexIn(data1, pos1)) != -1)
        {
            hash.insert(reg1.cap(2), reg1.cap(1));
            pos1 += reg1.matchedLength();
        }
        if (!hash.isEmpty())
            Ticket::GlobalTechs.insert(405, hash);

        hash.clear();
        data1 = reg.cap(3);
        pos1 = 0;
        while ((pos1 = reg1.indexIn(data1, pos1)) != -1)
        {
            hash.insert(reg1.cap(2), reg1.cap(1));
            pos1 += reg1.matchedLength();
        }
        if (!hash.isEmpty())
            Ticket::GlobalTechs.insert(-1, hash);
    }
}

void HtmlPigsClient::parseGroups(const QString & data)
{
    QRegExp reg("<select name=ncagroup[^>]*>[\\s]*((?:<option value=[\\d]+ >[^<]*</option>[\\s]*)+)</select>");
    if (reg.indexIn(data) != -1)
    {
        Ticket::Groups.clear();
        Ticket::Groups.insert(0, account.user);

        QString data1(reg.cap(1));
        QRegExp reg1("<option value=([\\d]+) >([^<]*)</option>");
        int pos1 = 0;

        while ((pos1 = reg1.indexIn(data1, pos1)) != -1)
        {
            Ticket::Groups.insert(reg1.cap(1).toInt(), reg1.cap(2));
            pos1 += reg1.matchedLength();
        }
    }
}

QMap<QString, QString> HtmlPigsClient::parseSelect(const QString & data)
{
    QMap<QString, QString> map;
    QRegExp reg("<option[\\s]+value=((?:[\"\'][^\"\']*[\"\'])|(?:[^\"\'\\s>]*))([\\s]*SELECTED)?[^>]*>([^<]*)</option>", Qt::CaseInsensitive);
    for (int pos = 0; (pos = reg.indexIn(data, pos)) != -1; pos += reg.matchedLength())
    {
        QString key = reg.cap(1).replace(QRegExp("(^[\"\'])|([\"\']$)"), "");
        if (!key.isEmpty())
        {
            map.insert(key, reg.cap(3));
            if (!reg.cap(2).isEmpty() && !map.contains(QString()))
                map.insert(QString(), key);
        }
    }
    return map;
}

QList<TicketInfo> HtmlPigsClient::parseTicketInfo(const QString & data)
{
    QRegExp one("<tr>\n<td[^>]*><a href=([^> ]*)>[^<]*</td>\n<td[^>]*>(\\d*)&nbsp;</td>\n<td[^>]*>([^&]*)&nbsp;</td>\n<td[^>]*>([^&]*)&nbsp;</td>\n<td[^>]*>([^&]*)&nbsp;</td>\n<td[^>]*>([^&]*)&nbsp;</td>\n<td[^>]*>([^&]*)&nbsp;</td>\n<td[^>]*>(\\d*)&nbsp;</td>\n<td[^>]*>([^&]*)&nbsp;</td>\n</tr>");
    QList<TicketInfo> out;
    int pos = 0;

    while((pos = one.indexIn(data, pos)) != -1) {
        out.push_back(TicketInfo(
                one.cap(1),
                one.cap(2),
                one.cap(3),
                one.cap(4),
                one.cap(5),
                one.cap(6),
                one.cap(7),
                one.cap(8).toInt(),
                one.cap(9).replace(QString("<b>"), QString()).replace(QString("</b>"), QString())
        ));
        pos += one.matchedLength();
    }

    return out;
}

QByteArray HtmlPigsClient::prepareQuery(const Ticket & ticket)
{
    QByteArray out;

    out.append(QByteArray("caseid=").append(QString::number(ticket.CaseID)));
    out.append(QByteArray("&nchostopian=").append(toPostText(ticket.Hostopian)));
    out.append(QByteArray("&hiddenhostopian=").append(toPostText(ticket.HostopianHidden)));
    out.append(QByteArray("&ncdomain=").append(toPostText(ticket.Domain)));
    out.append(QByteArray("&ncname=").append(toPostText(ticket.Name)));
    out.append(QByteArray("&ncemail=").append(toPostText(ticket.Email)));
    out.append(QByteArray("&ncategory=").append(toPostText(ticket.CategoryHidden)));
    out.append(QByteArray("&nsubcategory=").append(toPostText(ticket.SubCategoryHidden)));
    out.append(QByteArray("&category1=").append(QString::number(ticket.Category)));
    out.append(QByteArray("&subcategory1=").append(toPostText(ticket.SubCategory)));
    out.append(QByteArray("&ncproblem=").append(toPostText(ticket.Problem)));
    out.append(QByteArray("&hiddenproblem=").append(toPostText(ticket.ProblemHidden)));
    out.append(QByteArray("&ncpriority=").append(QString::number(ticket.Priority)));
    out.append(QByteArray("&hiddenpriority=").append(toPostText(ticket.PriorityHidden)));
    out.append(QByteArray("&ncstatus=").append(toPostText(ticket.Status)));
    out.append(QByteArray("&hiddenstatus=").append(toPostText(ticket.StatusHidden)));
    out.append(QByteArray("&oldstatus=").append(toPostText(ticket.StatusOld)));
    out.append(QByteArray("&ncnotes=").append(toPostText(ticket.Notes)));
    out.append(QByteArray("&hiddennotes=").append(toPostText(ticket.NotesHidden)));
    out.append(QByteArray("&ncresolution=").append(toPostText(ticket.Resolution)));
    out.append(QByteArray("&ncotech=").append(toPostText(ticket.OpenedByTech)));
    out.append(QByteArray("&ncagroup=").append(QString::number(ticket.AssignedGroup)));
    out.append(QByteArray("&showme=update"));
    out.append(QByteArray("&groupchange=").append(""));
    out.append(QByteArray("&techname=").append(toPostText(ticket.TechName)));
    out.append(QByteArray("&techemail=").append(toPostText(ticket.TechEmail)));
    out.append(QByteArray("&ncatech=").append(toPostText(ticket.AssignedTech.value(QString()))));
    out.append(QByteArray("&hiddenatech=").append(toPostText(ticket.AssignedTechHidden)));
    out.append(QByteArray("&uaudit=").append(toPostText(ticket.AuditTrail)));
    out.append(QByteArray("&stchange=").append(toPostText(ticket.StatusChange)));
    out.append(QByteArray("&ncaudit=").append(toPostText(ticket.AuditHidden)));

    return out;
}

QByteArray HtmlPigsClient::toPostText(const QString & text)
{
    return QUrl::toPercentEncoding(text, " ").replace(' ', '+');
}

void HtmlPigsClient::on_client_sslErrors(QNetworkReply * reply)
{
    reply->ignoreSslErrors();
}

void HtmlPigsClient::on_client_finished(QNetworkReply * reply)
{
    QNetworkRequest request(reply->request());
    PigsRequest req = (PigsRequest)(request.attribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 1)).toInt());
    int id = request.attribute((QNetworkRequest::Attribute)(QNetworkRequest::User + 2)).toInt();

    if(reply->error() != QNetworkReply::NoError) // if connection error
    {
        emit error(reply->errorString(), req, id);
    }
    else
    {
        QString cont(QTextCodec::codecForName("Windows-1252")->toUnicode(reply->readAll()));
        if(cont.indexOf(QString("<br><h2>Access denied. Please log in again.</h2>")) == 0) // if login not accepted
            emit error(tr("Login or password not accepted!"), req, id);
        else
            switch(req)
            {
                case RefreshGlobals:
                    parseHostopians(cont);
                    parseCategories(cont);
                    parseSubCategories(cont);
                    parseGlobalTechs(cont);
                    parseGroups(cont);
                    emit globalsReady();
                    break;
                case GetGroupTickets:
                    emit groupTicketsReady(id, parseTicketInfo(cont));
                    break;
                case SearchTickets:
                    emit searchReady(parseTicketInfo(cont));
                    break;
                case GetTicket:
                    {
                        Ticket t = parseTicket(cont);
                        if (t.CaseID == id)
                            emit ticketReady(t);
                        else
                            emit error(tr("Returned Case ID is incorrect: %1").arg(t.CaseID), req, id);
                    }
                    break;
                case GetDomainInfo:
                    {
                        int pos = 0;
                        emit domainInfoReady(subString(QString("<font color=red>(**domain name must match exactly)</font><br><br>"), QString("</center>"), pos, cont).trimmed());
                    }
                    break;
                case CreateTicket:
                    break;
                case UpdateTicket:
                    getTicket(id);
                    break;
                default:
                    emit error(tr("Unknown request"), req, id);
                    break;
            }
    }
    reply->deleteLater();
}

#include "WebKitPigsParser.h"

bool WebKitPigsParser::isAccessDeny(QWebFrame * frame)
{
    return (frame->evaluateJavaScript("document.body.children[1].textContent").toString().trimmed() == QString("Access denied. Please log in again."));
}

Ticket WebKitPigsParser::parseTicket(QWebFrame * frame)
{
    Ticket ticket;
    ticket.CaseID = frame->evaluateJavaScript("document.forms[0].children[0].children[0].children[0].children[1].childNodes[2].textContent").toString().trimmed().toInt();
    if (ticket.CaseID > 0)
    {
        ticket.DateOpened = frame->evaluateJavaScript("document.forms[0].children[0].children[0].children[1].children[0].childNodes[2].textContent").toString().trimmed();
        ticket.DateClosed = frame->evaluateJavaScript("document.forms[0].children[0].children[0].children[1].children[1].childNodes[1].textContent").toString().trimmed();
        ticket.Hostopian = frame->evaluateJavaScript("document.forms[0].nchostopian.value").toString();
        ticket.HostopianHidden = frame->evaluateJavaScript("document.forms[0].hiddenhostopian.value").toString();
        ticket.Domain = frame->evaluateJavaScript("document.forms[0].ncdomain.value").toString();
        ticket.Name = frame->evaluateJavaScript("document.forms[0].ncname.value").toString();
        ticket.Email = frame->evaluateJavaScript("document.forms[0].ncemail.value").toString();
        ticket.CategoryHidden = frame->evaluateJavaScript("document.forms[0].ncategory.value").toString();
        ticket.SubCategoryHidden = frame->evaluateJavaScript("document.forms[0].nsubcategory.value").toString();
        ticket.Category = frame->evaluateJavaScript("document.forms[0].category1.value").toString().toInt();
        ticket.SubCategory = frame->evaluateJavaScript("document.forms[0].subcategory1.value").toString();
        ticket.Problem = frame->evaluateJavaScript("document.forms[0].ncproblem.value").toString();
        ticket.ProblemHidden = frame->evaluateJavaScript("document.forms[0].hiddenproblem.value").toString();
        ticket.Priority = frame->evaluateJavaScript("document.forms[0].ncpriority.value").toString().toInt();
        ticket.PriorityHidden = frame->evaluateJavaScript("document.forms[0].hiddenpriority.value").toString();
        ticket.Status = frame->evaluateJavaScript("document.forms[0].ncstatus.value").toString();
        ticket.StatusHidden = frame->evaluateJavaScript("document.forms[0].hiddenstatus.value").toString();
        ticket.StatusOld = frame->evaluateJavaScript("document.forms[0].oldstatus.value").toString();
        ticket.Notes = frame->evaluateJavaScript("document.forms[0].ncnotes.value").toString();
        ticket.NotesHidden = frame->evaluateJavaScript("document.forms[0].hiddennotes.value").toString();
        ticket.Resolution = frame->evaluateJavaScript("document.forms[0].ncresolution.value").toString();
        ticket.OpenedByTech = frame->evaluateJavaScript("document.forms[0].ncotech.value").toString();
        ticket.AssignedGroup = frame->evaluateJavaScript("document.forms[0].ncagroup.value").toString().toInt();
        ticket.TechName = frame->evaluateJavaScript("document.forms[0].techname.value").toString();
        ticket.TechEmail = frame->evaluateJavaScript("document.forms[0].techemail.value").toString();
        ticket.ClosedByTech = frame->evaluateJavaScript("document.forms[0].children[0].children[0].children[20].children[0].childNodes[2].textContent").toString().trimmed();
        ticket.AssignedTech = parseSelect(frame, "document.forms[0].ncatech");
        ticket.AssignedTech.insert(QString(), frame->evaluateJavaScript("document.forms[0].ncatech.value").toString());
        ticket.AssignedTechHidden = frame->evaluateJavaScript("document.forms[0].hiddenatech.value").toString();
        ticket.AuditTrail = frame->evaluateJavaScript("document.forms[0].uaudit.value").toString();
        ticket.StatusChange = frame->evaluateJavaScript("document.forms[0].stchange.value").toString();
        ticket.AuditHidden = frame->evaluateJavaScript("document.forms[0].ncaudit.value").toString();
        ticket.AuditPrev = frame->evaluateJavaScript("document.forms[0].children[0].children[0].children[25].children[0].children[0].children[0].children[0].children[0].children[0].innerHTML").toString();
    }
    return ticket;
}

void WebKitPigsParser::parseHostopians(QWebFrame * frame)
{
    QMap<QString, QString> select(parseSelect(frame, "document.forms[0].nchostopian"));

    Ticket::Hostopians.clear();
    for (QMap<QString, QString>::const_iterator iter = select.constBegin(); iter != select.constEnd(); iter++)
    {
        Ticket::Hostopians.append(iter.key());
    }
}

void WebKitPigsParser::parseCategories(QWebFrame * frame)
{
    QMap<QString, QString> select(parseSelect(frame, "document.forms[0].category1"));

    Ticket::Categories.clear();
    for (QMap<QString, QString>::const_iterator iter = select.constBegin(); iter != select.constEnd(); iter++)
    {
        Ticket::Categories.insert(iter.key().toInt(), iter.value());
    }
}

void WebKitPigsParser::parseSubCategories(QWebFrame * frame)
{
    QString data(frame->evaluateJavaScript("setOptions.toString()").toString());
    if (data.length() > 0)
    {
        Ticket::SubCategories.clear();

        QRegExp reg1("if \\(chosen == [\"']([^'\"]*)[\"']\\)[\\s]*\\{[\\s\\w\\d/]*((?:selbox\\.options\\[selbox\\.options\\.length\\] = new Option\\('[^']*','[^']*'\\);[\\s]*)+)\\}");
        int pos1 = 0;
        while ((pos1 = reg1.indexIn(data, pos1)) != -1)
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

void WebKitPigsParser::parseGlobalTechs(QWebFrame * frame)
{
    QString data(frame->evaluateJavaScript("setOptions2.toString()").toString());
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

void WebKitPigsParser::parseGroups(QWebFrame * frame)
{
    QMap<QString, QString> select(parseSelect(frame, "document.forms[0].ncagroup"));

    Ticket::Groups.clear();
    for (QMap<QString, QString>::const_iterator iter = select.constBegin(); iter != select.constEnd(); iter++)
    {
        Ticket::Groups.insert(iter.key().toInt(), iter.value());
    }
}

QMap<QString, QString> WebKitPigsParser::parseSelect(QWebFrame * frame, const QString & node)
{
    QMap<QString, QString> map;

    int length = (int)frame->evaluateJavaScript(node + ".length").toDouble();

    for (int i = 0; i < length; i++)
    {
        QString key(frame->evaluateJavaScript(node + QString(".options[%1].value").arg(i)).toString());
        if (!key.isEmpty())
        {
            map.insert(key, frame->evaluateJavaScript(node + QString(".options[%1].text").arg(i)).toString());
        }
    }

    return map;
}

QList<TicketInfo> WebKitPigsParser::parseTicketInfo(QWebFrame * frame, PigsClient::PigsRequest requestType)
{
    QString node;

    switch (requestType)
    {
        case PigsClient::SearchTickets:
            node.append("document.body.children[6].children[0].children[0].children");
            break;
        case PigsClient::GetGroupTickets:
            node.append("document.body.children[5].children[0].children[0].children");
            break;
        default:
            break;
    }

    int length = (int)frame->evaluateJavaScript(node + ".length").toDouble();

    QList<TicketInfo> out;

    for (int i = 1; i < length; i++)
    {
        out.append(TicketInfo(
                QUrl(frame->evaluateJavaScript(node + QString("[%1].children[0].children[0].href").arg(i)).toString()),
                frame->evaluateJavaScript(node + QString("[%1].children[1].textContent").arg(i)).toString().trimmed(),
                frame->evaluateJavaScript(node + QString("[%1].children[2].textContent").arg(i)).toString().trimmed(),
                frame->evaluateJavaScript(node + QString("[%1].children[3].textContent").arg(i)).toString().trimmed(),
                frame->evaluateJavaScript(node + QString("[%1].children[4].textContent").arg(i)).toString().trimmed(),
                frame->evaluateJavaScript(node + QString("[%1].children[5].textContent").arg(i)).toString().trimmed(),
                frame->evaluateJavaScript(node + QString("[%1].children[6].textContent").arg(i)).toString().trimmed(),
                frame->evaluateJavaScript(node + QString("[%1].children[7].textContent").arg(i)).toString().trimmed().toInt(),
                frame->evaluateJavaScript(node + QString("[%1].children[8].textContent").arg(i)).toString().trimmed()
                ));
    }

    return out;
}

QString WebKitPigsParser::parseDomainInfo(QWebFrame * frame)
{
    return frame->evaluateJavaScript("document.body.children[5].children[1].children[7].innerHTML").toString().trimmed();
}

QByteArray WebKitPigsParser::prepareQuery(const Ticket & ticket)
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

QByteArray WebKitPigsParser::toPostText(const QString & text)
{
    return QUrl::toPercentEncoding(text, " ").replace(' ', '+');
}



#ifndef WEBKITPIGSPARSER_H
#define WEBKITPIGSPARSER_H

#include "PigsClient.h"
#include <QWebFrame>

class WebKitPigsParser
{
private:
    WebKitPigsParser() {}

public:
    static bool isAccessDeny(QWebFrame * frame);
    static Ticket parseTicket(QWebFrame * frame);
    static void parseHostopians(QWebFrame * frame);
    static void parseCategories(QWebFrame * frame);
    static void parseSubCategories(QWebFrame * frame);
    static void parsePriorities(QWebFrame * frame);
    static void parseStatuses(QWebFrame * frame);
    static void parseGlobalTechs(QWebFrame * frame);
    static void parseGroups(QWebFrame * frame);
    static QMap<QString, QString> parseSelect(QWebFrame * frame, const QString & node);
    static QList<TicketInfo> parseTicketInfo(QWebFrame * frame, PigsClient::PigsRequest requestType);
    static QString parseDomainInfo(QWebFrame * frame);

    static QByteArray prepareQuery(const Ticket & ticket);
    static QByteArray toPostText(const QString & text);
};

#endif // WEBKITPIGSPARSER_H

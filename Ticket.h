#ifndef TICKET_H
#define TICKET_H

#include <QtCore/QHash>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>
#include <QtCore/QUrl>


typedef QMap<QString, QString> QStringMap;
typedef QHash<QString, QString> QStringHash;

class Ticket
{
public:
    static QStringList Hostopians;
    static QMap<int, QString> Categories;
    static QHash<int, QStringList> SubCategories;
    static QList<int> Priorities;
    static QMap<QString, QString> Statuses;
    static QMap<int, QString> Groups;
    static QHash<int, QStringMap> GlobalTechs;

    Ticket();

    /*
    int caseId() const { return CaseID; }
    const QString & dateOpened() const { return DateOpened; }
    const QString & dateClosed() const { return DateClosed; }
    const QString & hostopian() const { return Hostopian; }
    const QString & hostopianHidden() const { return HostopianHidden; }
    const QString & domain() const { return Domain; }
    const QString & customerName() const { return Name; }
    const QString & customerEmail() const { return Email; }
    int category() const { return Category; }
    const QString & categoryHidden() const { return CategoryHidden; }
    */

    bool isValid() const;
    void setStatus(const QString & status);

public:
    int CaseID;
    QString DateOpened;
    QString DateClosed;
    QString Hostopian;
    QString HostopianHidden;
    QString Domain;
    QString Name;
    QString Email;
    //QMap<QString, QString> CategoryList;
    int Category;
    QString CategoryHidden;
    //QMap<QString, QString> SubCategoryList;
    QString SubCategory;
    QString SubCategoryHidden;
    QString Problem;
    QString ProblemHidden;
    int Priority;
    QString PriorityHidden;
    QString Status;
    QString StatusHidden;
    QString StatusOld;
    QString StatusChange;
    QString Notes;
    QString NotesHidden;
    QString Resolution;
    QString OpenedByTech;
    QString ClosedByTech;
    int AssignedGroup;
    QString TechName;
    QString TechEmail;
    //QMap<QString, QString> AssignedTechList;
    QMap<QString, QString> AssignedTech;
    QString AssignedTechHidden;
    QString AuditTrail;
    QString AuditHidden;
    QString AuditPrev;
};

#endif // TICKET_H

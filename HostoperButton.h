#ifndef CHOSTOPERBUTTON_H
#define CHOSTOPERBUTTON_H

#include <QObject>
#include <QtCore/QUrl>
#include <QtCore/QTime>
#include <QtGui/QPushButton>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include "SAccount.h"

class HostoperButton : public QPushButton
{
    Q_OBJECT

public:
    HostoperButton(QWidget * = 0);
    ~HostoperButton();
    void setAccount(const Account & account);
    Account getAccount() const { return account; }

public slots:
    void check(bool = false);

private slots:
    void on_clicked();
    void on_error();
    void on_client_finished(QNetworkReply *);

signals:
    void error(QString);

private:
    Account account;
    QTime startTime;
    QTime endTime;
    QTime currTime;
    QNetworkAccessManager * client;
    bool doSubmit;
    bool onWork;
    bool isBusy;

    enum RequestType {
        Unknown,
        LogIn,
        TimeBoardIndex,
        TimeBoardWorkboard,
        LogOut
    };

    const static QString dayReg;
    const static QUrl domain;
    const static QUrl tbIndex;
    const static QUrl tbWorkboard;
    const static QUrl tbLogout;

    void get(RequestType, QNetworkRequest);
    void post(RequestType, QNetworkRequest, const QByteArray &);
    void submit();
    bool parseHTML(const QString &);

protected:
    virtual void mousePressEvent(QMouseEvent *);
};

#endif // CHOSTOPERBUTTON_H

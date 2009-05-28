#include "HostoperButton.h"
#include <QMouseEvent>
#include <QRegExp>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QLocale>

const QString HostoperButton::dayReg("<td align=\"center\" valign=\"bottom\" height=\"60\" class=\"[rl] cell t\" background=\"\\.\\.\\/images\\/((?:\\d)+).gif\" style=\"background-position: top; background-repeat: no-repeat;\"><span class=\"time\">(?:(?:&nbsp;)?(\\d\\d:\\d\\d)-(?:\\.\\.\\. |(\\d\\d:\\d\\d)))?&nbsp;</td>");
const QUrl HostoperButton::domain("http://ua.hostopia.com/");
const QUrl HostoperButton::tbIndex("http://ua.hostopia.com/timeboard/index.php");
const QUrl HostoperButton::tbWorkboard("http://ua.hostopia.com/timeboard/workboard.php?UseLang=Ru");
const QUrl HostoperButton::tbLogout("http://ua.hostopia.com/index.php?action=logout");


HostoperButton::HostoperButton(QWidget * parent):
        QPushButton("...", parent), doSubmit(false), onWork(false), isBusy(false)
{
    client = new QNetworkAccessManager(this);
    this->setCursor(QCursor(Qt::PointingHandCursor));
    this->setEnabled(false);
    this->setVisible(false);
    connect(this, SIGNAL(clicked(bool)), SLOT(on_clicked()));
    connect(this, SIGNAL(error(QString)), SLOT(on_error()));
    connect(client, SIGNAL(finished(QNetworkReply *)), SLOT(on_client_finished(QNetworkReply *)));
}

void HostoperButton::setAccount(const Account & acc)
{
    if (!acc.isValid()) {
        account.clear();
        this->setText("...");
        this->setToolTip(QString());
        this->setStatusTip(QString());
        this->setEnabled(false);
        this->setVisible(false);
    } else {
        account = acc;
        this->setToolTip(acc.user);
        this->setStatusTip(acc.user);
        this->setEnabled(false);
        this->setVisible(true);
        check(false);
    }
}

void HostoperButton::check(bool submit)
{
    if(!isBusy) {
        isBusy = true;
        doSubmit = submit;
        QByteArray postData = QByteArray("username=").append(QUrl::toPercentEncoding(account.user)).append("&password=").append(QUrl::toPercentEncoding(account.pass));

        QNetworkRequest nr(domain);
        nr.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(QString("application/x-www-form-urlencoded")));
        nr.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(postData.length()));

        this->setEnabled(false);
        post(LogIn, nr, postData);
    }
}

void HostoperButton::on_clicked()
{
    check(true);
}

void HostoperButton::on_error()
{
    this->setText("...");
    this->setEnabled(false);
    isBusy = false;
}

void HostoperButton::get(RequestType st, QNetworkRequest request)
{
    request.setAttribute(QNetworkRequest::User, QVariant((int)st));
    client->get(request);
}

void HostoperButton::post(RequestType st, QNetworkRequest request, const QByteArray & data) {
    request.setAttribute(QNetworkRequest::User, QVariant((int)st));
    client->post(request, data);
}

void HostoperButton::submit() {
    QByteArray postData;
    if(onWork) postData.append("endday=");
    else postData.append("stday=");

    QNetworkRequest nr(tbWorkboard);
    nr.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(QString("application/x-www-form-urlencoded")));
    nr.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(postData.length()));

    doSubmit = false;
    post(TimeBoardWorkboard, nr, postData);
}

void HostoperButton::on_client_finished(QNetworkReply * reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        emit error(reply->errorString());
    }
    else {
        switch((RequestType)reply->request().attribute(QNetworkRequest::User).toInt())
        {
            case LogIn:
                if(!reply->rawHeader("Location").contains("error.php"))
                    get(TimeBoardIndex, QNetworkRequest(tbIndex));
                else
                    emit error(tr("Login or password not accepted!"));
                break;

            case TimeBoardIndex:
                get(TimeBoardWorkboard, QNetworkRequest(tbWorkboard));
                break;

            case TimeBoardWorkboard:
                currTime = QLocale(QLocale::C).toDateTime(QString(reply->rawHeader("Date")), "ddd, dd MMM yyyy hh:mm:ss 'GMT'").time();
                if (currTime.isValid())
                    currTime = currTime.addSecs(7200);

                if (!parseHTML(reply->readAll()))
                    emit error(tr("Web page was modified. Can't find correct data!"));

                if(doSubmit)
                    submit();
                else
                    get(LogOut, QNetworkRequest(tbLogout));
                break;

            case LogOut:
                {
                    QString txt;
                    if(startTime.isValid()) {
                        txt.append(QString("%1").arg(startTime.toString("hh:mm")));
                        if(endTime.isValid())
                            txt.append(QString("-%1").arg(endTime.toString("hh:mm")));
                        else
                            txt.append("-...");
                    } else
                        txt.append("...");
                    this->setText(txt);
                    this->setEnabled(true);
                    isBusy = false;
                }
                break;
            default:
                emit error(tr("Unknown request"));
                break;
        }
    }
    reply->deleteLater();
}

bool HostoperButton::parseHTML(const QString &cont)
{
    if(cont.contains("<input type=\"submit\" name=\"endday\" class=\"endday\" value=\"\">")) onWork = true;
    else onWork = false;

    QRegExp reg(dayReg);

    if(reg.indexIn(cont) != -1) {
        startTime = QTime::fromString(reg.cap(2), "hh:mm");
        endTime = QTime::fromString(reg.cap(3), "hh:mm");
        return true;
    }
    return false;
}

void HostoperButton::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::MidButton) check(false);
    QPushButton::mousePressEvent(event);
}

HostoperButton::~HostoperButton()
{
}

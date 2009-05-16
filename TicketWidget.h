#ifndef TICKETWIDGET_H
#define TICKETWIDGET_H

#include <QtCore/QHash>
#include <QtGui/QWidget>
#include <QtGui/QMenu>
#include <QtGui/QMovie>
#include "Ticket.h"

namespace Ui {
    class TicketWidget;
}

class TicketWidget : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(TicketWidget)
public:
    explicit TicketWidget(int id, QWidget *parent = 0);
    virtual ~TicketWidget();

    void updateContent(const Ticket & ticket);
    int getID() { return ticket.CaseID; }

signals:
    void updateTicket(Ticket & ticket);
    void getDomainInfo(const QString & domain);

private:
    Ui::TicketWidget *m_ui;
    Ticket ticket;

private slots:
    void on_toolButton_clicked();
    void on_pushButton_Reset_clicked();
    void on_comboBox_Category_currentIndexChanged(int index);
    void on_comboBox_Group_currentIndexChanged(int index);
    void on_pushButton_Update_clicked();

protected:
    virtual void changeEvent(QEvent *e);
};

#endif // TICKETWIDGET_H

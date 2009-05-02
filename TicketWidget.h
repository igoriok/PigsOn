#ifndef TICKETWIDGET_H
#define TICKETWIDGET_H

#include <QtCore/QHash>
#include <QtGui/QWidget>
#include <QtGui/QMenu>
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

private:
    Ui::TicketWidget *m_ui;
    Ticket ticket;

private slots:
    void on_pushButton_Reset_clicked();
    void on_comboBox_Category_currentIndexChanged(int index);
    void on_comboBox_Group_currentIndexChanged(int index);
    void on_pushButton_Update_clicked();

protected:
    virtual void changeEvent(QEvent *e);

signals:
    void updateTicket(Ticket & ticket);
};

#endif // TICKETWIDGET_H

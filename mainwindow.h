#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QTranslator>
#include <QtGui/QMainWindow>
#include <QtGui/QSystemTrayIcon>
#include <QtCore/QTimer>
#include <QtCore/QQueue>
#include "ui_mainwindow.h"
#include "SettingsManager.h"
#include "HostoperButton.h"
#include "HtmlPigsClient.h"
#include "TicketWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~MainWindow();

    TicketWidget * findTicketWidget(int id);
    TicketWidget * createTicketWidget(int id);

    QTreeWidgetItem * findGroupItem(int id);

private:
    Ui::MainWindowClass ui;
    QMultiMap<int, QList<TicketInfo> > lastTickets;
    QQueue<QPair<QString, QString> > trayMessages;

    QTimer * timer;
    QSystemTrayIcon * tray;
    QTranslator * appTranslator;

    PigsClient * pigser;
    HostoperButton * hostoper;
    SettingsManager * settings;

private slots:
    // Gui Events
    void on_dockWidget_Cases_dockLocationChanged(Qt::DockWidgetArea area);
    void on_actionRussian_toggled(bool);
    void on_actionEnglish_toggled(bool);
    void on_tableWidget_customContextMenuRequested(QPoint pos);
    void on_tableWidget_itemDoubleClicked(QTableWidgetItem* item);
    void on_tabWidget_tabCloseRequested(int index);
    void on_lineEdit_returnPressed();
    void on_actionSettings_triggered();
    void on_actionReloadAllGroups_triggered();
    void on_actionReloadTicket_triggered();
    void on_actionCloseTicket_triggered();
    void on_actionSearchTickets_triggered();
    void on_actionNewTicket_triggered();
    void on_actionAbout_triggered();
    void on_actionAboutQt_triggered();
    void on_actionQuit_triggered();
    void on_treeWidget_customContextMenuRequested(const QPoint & pos);
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem * item);
    void on_tabWidget_currentChanged(int index);
    void on_tray_activated(QSystemTrayIcon::ActivationReason);

    // Ticket Widget Events
    void updateTicket(Ticket & ticket);
    void getDomainInfo(const QString & domain);

    // Pigser Events
    void on_pigser_globalsReady();
    void on_pigser_groupTicketsReady(int id, const QList<TicketInfo> & tickets);
    void on_pigser_searchReady(const QList<TicketInfo> & tickets);
    void on_pigser_ticketReady(const Ticket & ticket);
    void on_pigser_domainInfoReady(const QString & info);
    void on_pigser_error(QString error, PigsClient::PigsRequest req, int id);
    void on_pigser_showMessage(QString mess);

    // Hostoper Event
    void on_hostoper_error(QString error);

    // Settings Event
    void on_settings_settingsChanged();

    // Misc Events
    void clearStatusBar();
    void openTicketWidget(int id);
    void showTrayMessage();

protected:
    virtual void changeEvent(QEvent *);
};

#endif // MAINWINDOW_H

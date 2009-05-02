#include "mainwindow.h"
#include "ui_SearchDialog.h"
#include <QtGui/QMessageBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    timer = new QTimer(this);
    timer->setObjectName("timer");

    tray = new QSystemTrayIcon(QIcon(":/res/cash.png"), this);
    tray->setObjectName("tray");

    settings = new SettingsManager(this);
    settings->setObjectName("settings");

    pigser = new PigsClient(this);
    pigser->setObjectName("pigser");

    hostoper = new HostoperButton(this);
    hostoper->setObjectName("hostoper");

    appTranslator = new QTranslator(this);
    appTranslator->load(":/ru.qm");

    ui.setupUi(this);

    this->connect(pigser, SIGNAL(groupTicketsReady(int,QList<TicketInfo>)), SLOT(clearStatusBar()));
    this->connect(pigser, SIGNAL(ticketReady(Ticket)), SLOT(clearStatusBar()));
    this->connect(pigser, SIGNAL(searchReady(QList<TicketInfo>)), SLOT(clearStatusBar()));
    this->connect(pigser, SIGNAL(error(QString,PigsRequest,int)), SLOT(clearStatusBar()));

    ui.menuView->addAction(ui.dockWidget_Cases->toggleViewAction());
    ui.menuView->addAction(ui.dockWidget_Search->toggleViewAction());
    ui.menuToolbar->addAction(ui.toolBar_TicketWidget->toggleViewAction());

    this->connect(timer, SIGNAL(timeout()), ui.actionReloadAllGroups, SLOT(trigger()));

    QMenu * tMenu = new QMenu(this);
    tMenu->addAction(ui.actionReloadAllGroups);
    tMenu->addSeparator();
    tMenu->addAction(ui.actionQuit);
    tray->setContextMenu(tMenu);

    ui.lineEdit->setValidator(new QIntValidator(1, INT_MAX, ui.lineEdit));
    ui.lineEdit->connect(ui.actionOpenTicket, SIGNAL(triggered()), SLOT(show()));
    ui.lineEdit->connect(ui.lineEdit, SIGNAL(editingFinished()), SLOT(hide()));
    ui.lineEdit->hide();

    ui.statusBar->addPermanentWidget(hostoper);

    ui.actionRussian->connect(ui.actionRussian, SIGNAL(toggled(bool)), SLOT(setDisabled(bool)));
    ui.actionEnglish->connect(ui.actionEnglish, SIGNAL(toggled(bool)), SLOT(setDisabled(bool)));

    settings->loadConfig();

    this->restoreState(settings->getMainWindowState());

    QList<int> gr(settings->getPigsGroups());
    for (int i = 0; i < gr.size(); ++i)
    {
        QTreeWidgetItem * root = new QTreeWidgetItem(ui.treeWidget, QStringList(QString("Unknown [%1]").arg(gr.at(i))));
        root->setData(0, Qt::UserRole, QVariant(false));
        root->setData(0, Qt::UserRole + 1, QVariant(gr.at(i)));
        root->setHidden(true);
    }

    if (settings->getLang())
        ui.actionRussian->setChecked(true);
}

TicketWidget * MainWindow::createTicketWidget(int id)
{
    TicketWidget * tw = new TicketWidget(id);
    this->connect(tw, SIGNAL(updateTicket(Ticket &)), SLOT(updateTicket(Ticket &)));
    return tw;
}

TicketWidget * MainWindow::findTicketWidget(int id)
{
    for (int i = 0; i < ui.tabWidget->count(); i++)
    {
        QWidget * w = ui.tabWidget->widget(i);
        if (w != NULL)
        {
            TicketWidget * tw = (TicketWidget *)w;
            if (tw->getID() == id)
            {
                return tw;
            }
        }
    }
    return NULL;
}

void MainWindow::openTicketWidget(int id)
{
    if (id > 0)
    {
        TicketWidget * tw = findTicketWidget(id);
        if (tw == NULL)
        {
            ui.tabWidget->setCurrentIndex(ui.tabWidget->addTab(createTicketWidget(id), QIcon(":/res/redo.png"), QString::number(id)));
            pigser->getTicket(id);
        }
        else
            ui.tabWidget->setCurrentWidget(tw);
    }
}

QTreeWidgetItem * MainWindow::findGroupItem(int id)
{
    QTreeWidgetItem * root = ui.treeWidget->invisibleRootItem();
    for (int i = 0; i < root->childCount(); i++)
    {
        QTreeWidgetItem * item = root->child(i);
        if (item->data(0, Qt::UserRole + 1).toInt() == id)
            return item;
    }
    return NULL;
}

void MainWindow::on_lineEdit_returnPressed()
{
    int id = ui.lineEdit->text().toInt();
    if (id > 0)
    {
        TicketWidget * tw = findTicketWidget(id);
        if (tw == NULL)
        {
            ui.tabWidget->setCurrentIndex(ui.tabWidget->addTab(createTicketWidget(id), QIcon(":/res/redo.png"), QString::number(id)));
            pigser->getTicket(id);
        }
        else
            ui.tabWidget->setCurrentWidget(tw);
    }
}

void MainWindow::updateTicket(Ticket & ticket)
{
    TicketWidget * tw = findTicketWidget(ticket.CaseID);
    if (tw != NULL)
    {
        ui.tabWidget->setTabIcon(ui.tabWidget->indexOf(tw), QIcon(":/res/redo.png"));
        if (ui.tabWidget->currentWidget() == tw)
        {
            ui.actionReloadTicket->setEnabled(false);
        }
        pigser->updateTicket(ticket);
    }
}

void MainWindow::on_pigser_globalsReady()
{
    QList<int> groups(settings->getPigsGroups());
    QTreeWidgetItem * root = ui.treeWidget->invisibleRootItem();
    for (int i = 0; i < root->childCount(); ++i)
    {
        QTreeWidgetItem * item = root->child(i);
        int id = item->data(0, Qt::UserRole + 1).toInt();
        if (Ticket::Groups.contains(id))
            item->setText(0, QString("%1 (%2)").arg(Ticket::Groups.value(id)).arg(item->childCount()));
    }
    clearStatusBar();
    ui.actionReloadAllGroups->trigger();
}

void MainWindow::on_pigser_groupTicketsReady(int id, const QList<TicketInfo> & tickets)
{
    if (id >= 0)
    {
        QList<int> prev;
        int newTickets = 0;
        QString gText(QString("%1 (%2)").arg(Ticket::Groups.value(id, QString("Unknown [%1]").arg(id))).arg(tickets.size()));
        QTreeWidgetItem * root = findGroupItem(id);
        if (root == NULL)
        {
            root = new QTreeWidgetItem(QStringList(gText));
            root->setData(0, Qt::UserRole, QVariant(false));
            root->setData(0, Qt::UserRole + 1, QVariant(id));
            ui.treeWidget->addTopLevelItem(root);
        }
        else
        {
            while (root->childCount() > 0)
            {
                QTreeWidgetItem * item = root->child(0);
                root->removeChild(item);
                if (item->backgroundColor(0) != QColor(0, 255, 0))
                    prev.append(item->data(0, Qt::UserRole + 1).toInt());
                delete item;
            }
        }

        root->setText(0, gText);
        root->setToolTip(0, gText);
        root->setStatusTip(0, gText);
        root->setDisabled(true);

        for (int i = 0; i < tickets.size(); i++) {
            const TicketInfo & tk = tickets.at(i);
            int caseID = tk.getCaseID().toInt();
            if (caseID > 0)
            {
                QTreeWidgetItem * item = new QTreeWidgetItem(root, QStringList(tk.getCaseID())<<tk.getCategory()<<tk.getHostopian()<<tk.getName()<<tk.getDomain()<<tk.getStatus()<<QString::number(tk.getPriority())<<tk.getOpened());

                item->setData(0, Qt::UserRole, QVariant(true));
                item->setData(0, Qt::UserRole + 1, QVariant(caseID));

                QString tt;
                tt.append(QString("CaseID:\t %1\n").arg(tk.getCaseID()));
                tt.append(QString("Category:\t %1\n").arg(tk.getCategory()));
                tt.append(QString("Hostopian:\t %1\n").arg(tk.getHostopian()));
                tt.append(QString("Name:\t %1\n").arg(tk.getName()));
                tt.append(QString("Domain:\t %1\n").arg(tk.getDomain()));
                tt.append(QString("Status:\t %1\n").arg(tk.getStatus()));
                tt.append(QString("Priority:\t %1\n").arg(tk.getPriority()));
                tt.append(QString("Opened:\t %1\n").arg(tk.getOpened()));

                item->setToolTip(0, tt);

                int j;
                for (j = 0; j < prev.size(); j++)
                    if (caseID == prev.at(j)) break;
                if (tk.getStatus() == QString("in_progress"))
                    for (int k = 0; k < item->columnCount(); ++k)
                        item->setBackgroundColor(k, QColor(255, 255, 0));
                else if (j == prev.size())
                {
                    newTickets++;
                    for (int k = 0; k < item->columnCount(); ++k)
                        item->setBackgroundColor(k, QColor(0, 255, 0));
                }
            }
        }

        ui.treeWidget->resizeColumnToContents(0);
        root->setDisabled(false);
        root->setHidden(false);

        if (newTickets && this->isHidden() && settings->showTrayMessages())
        {
            if (trayMessages.size() == 0)
                QTimer::singleShot(3000, this, SLOT(showTrayMessage()));
            trayMessages.enqueue(QPair<QString, QString>(Ticket::Groups.value(id), tr("%1 new ticket(s)").arg(newTickets)));
        }
    }
    else
        QMessageBox::information(this, tr("Error"), tr("Invalid Group ID [%1]").arg(id));
}

void MainWindow::on_pigser_searchReady(const QList<TicketInfo> & tickets)
{
    ui.tableWidget->setRowCount(ui.tableWidget->rowCount() + tickets.size());
    for (int i = 0; i < tickets.size(); ++i)
    {
        const TicketInfo & ticket = tickets.at(i);
        ui.tableWidget->setItem(i, 0, new QTableWidgetItem(ticket.getCaseID()));
        ui.tableWidget->setItem(i, 1, new QTableWidgetItem(ticket.getCategory()));
        ui.tableWidget->setItem(i, 2, new QTableWidgetItem(ticket.getHostopian()));
        ui.tableWidget->setItem(i, 3, new QTableWidgetItem(ticket.getName()));
        ui.tableWidget->setItem(i, 4, new QTableWidgetItem(ticket.getDomain()));
        ui.tableWidget->setItem(i, 5, new QTableWidgetItem(ticket.getStatus()));
        ui.tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(ticket.getPriority())));
    }
    ui.tableWidget->resizeColumnsToContents();
    ui.tableWidget->setEnabled(true);
    ui.dockWidget_Search->setVisible(true);
    ui.actionSearchTickets->setEnabled(true);
}

void MainWindow::on_pigser_ticketReady(const Ticket & ticket)
{
    if(ticket.CaseID > 0)
    {
        TicketWidget * tw = findTicketWidget(ticket.CaseID);
        if (tw != NULL)
        {
            int tabI = ui.tabWidget->indexOf(tw);
            tw->updateContent(ticket);
            ui.tabWidget->setTabIcon(tabI, QIcon(":/res/doc.png"));
            ui.tabWidget->setTabEnabled(tabI, true);

            if (ui.tabWidget->currentWidget() == tw)
            {
                ui.actionReloadTicket->setEnabled(true);
            }
        }
        //else
            //QMessageBox::information(this, tr("Error"), tr("Ticket Widget not finded!"));
    }
    else
        QMessageBox::information(this, tr("Error"), tr("Invalid Case ID [%1]").arg(ticket.CaseID));
}

void MainWindow::on_pigser_error(QString error, PigsRequest req, int id)
{
    QMessageBox::information(this, tr("Pigser"), error);
    switch(req)
    {
        case SearchTickets:
            ui.actionSearchTickets->setEnabled(true);
            break;
        default:
            break;
    }
}

void MainWindow::on_pigser_showMessage(QString mess)
{
    ui.statusBar->showMessage(mess);
}

void MainWindow::on_hostoper_error(QString error)
{
    QMessageBox::information(this, tr("Hostoper"), error);
}

void MainWindow::on_settings_settingsChanged()
{
    pigser->setAccount(settings->getPigsAccount());
    hostoper->setAccount(settings->getHostopAccount());
    timer->setInterval(settings->getTimerInterval() * 60000);

    if (settings->showTrayIcon()) tray->show();
    else tray->hide();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index != -1)
    {
        if (ui.tabWidget->widget(index)->isEnabled())
            ui.actionReloadTicket->setEnabled(true);
        else
            ui.actionReloadTicket->setEnabled(false);
        ui.actionCloseTicket->setEnabled(true);
    }
    else
    {
        ui.actionReloadTicket->setEnabled(false);
        ui.actionCloseTicket->setEnabled(false);
    }
}

void MainWindow::on_treeWidget_customContextMenuRequested(const QPoint & pos)
{
    QPoint pt(ui.treeWidget->mapToGlobal(pos));
    if (!ui.treeWidget->isHeaderHidden())
        pt += QPoint(0, ui.treeWidget->header()->height());

    QMenu menu(ui.treeWidget);
    QMenu * addGroup = NULL;
    QAction * removeGroup = NULL;
    QAction * reloadGroup = NULL;
    QAction * editTicket = NULL;
    QAction * markRead = NULL;
    QAction * markAllRead = NULL;

    QTreeWidgetItem * item = ui.treeWidget->itemAt(pos);
    if (item != NULL)
    {
        if (item->data(0, Qt::UserRole).toBool())
        {
            editTicket = menu.addAction(QIcon(":/res/paste.png"), tr("Edit Ticket"));
            if (item->backgroundColor(0) == QColor(0, 255, 0))
                markRead = menu.addAction(QIcon(":/res/paste.png"), tr("Mark as Read"));
            menu.addSeparator();
        } else
        {
            reloadGroup = menu.addAction(QIcon(":/res/redo.png"), tr("Reload Group"));
            removeGroup = menu.addAction(QIcon(":/res/minus.png"), tr("Remove Group"));
            if (!item->isDisabled() && item->childCount() > 0)
                markAllRead = menu.addAction(QIcon(":/res/paste.png"), tr("Mark All as Read"));
            menu.addSeparator();
        }
    }

    addGroup = menu.addMenu(QIcon(":/res/plus.png"), tr("Add Group"));
    int items = 0;
    for (QMap<int, QString>::const_iterator iter = Ticket::Groups.constBegin(); iter != Ticket::Groups.constEnd(); ++iter)
    {
        if (iter.key() > 0)
        {
            if (findGroupItem(iter.key()) == NULL)
            {
                addGroup->addAction(iter.value())->setData(QVariant(iter.key()));
                items++;
            }
        }
    }
    if (!items) addGroup->addAction(tr("None"))->setEnabled(false);

    menu.addAction(ui.actionReloadAllGroups);

    QAction * result = menu.exec(pt);

    if (result != NULL)
    {
        if (result->parentWidget() == addGroup)
        {
            int grID = result->data().toInt();
            QTreeWidgetItem * root = new QTreeWidgetItem(QStringList(Ticket::Groups.value(grID, tr("Unknown [%1]").arg(grID))));
            root->setData(0, Qt::UserRole, QVariant(false));
            root->setData(0, Qt::UserRole + 1, QVariant(grID));
            ui.treeWidget->addTopLevelItem(root);
            pigser->getGroupTickets(grID);
        }
        else if (result == removeGroup)
        {
            ui.treeWidget->invisibleRootItem()->removeChild(item);
        }
        else if (result == reloadGroup)
        {
            item->setDisabled(true);
            pigser->getGroupTickets(item->data(0, Qt::UserRole + 1).toInt());
        }
        else if (result == editTicket)
        {
            openTicketWidget(item->data(0, Qt::UserRole + 1).toInt());
        }
        else if (result == markRead)
            for (int k = 0; k < item->columnCount(); ++k)
                item->setBackgroundColor(k, QColor(255, 255, 255));
        else if (result == markAllRead)
            for (int i = 0; i < item->childCount(); ++i)
            {
                if (item->child(i)->backgroundColor(0) == QColor(0, 255, 0))
                    for (int k = 0; k < item->child(i)->columnCount(); ++k)
                        item->child(i)->setBackgroundColor(k, QColor(255, 255, 255));
            }
    }
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem * item)
{
    if (item->data(0, Qt::UserRole).toBool())
    {
        if (item->backgroundColor(0) == QColor(0, 255, 0))
            for (int k = 0; k < item->columnCount(); ++k)
                item->setBackgroundColor(k, QColor(255, 255, 255));
        openTicketWidget(item->data(0, Qt::UserRole + 1).toInt());
    }
}

void MainWindow::on_tableWidget_customContextMenuRequested(QPoint pos)
{
    QPoint pt(ui.tableWidget->mapToGlobal(pos));
    pt += QPoint(0, ui.tableWidget->horizontalHeader()->height());

    QMenu menu(ui.tableWidget);
    QAction * edit = NULL;
    QAction * clear = NULL;
    QTableWidgetItem * item = ui.tableWidget->itemAt(pos);
    int id = 0;
    if (item != NULL)
    {
        edit = menu.addAction(QIcon(":/res/paste.png"), tr("Edit Ticket"));
        id = ui.tableWidget->item(item->row(), 0)->text().toInt();
    }
    clear = menu.addAction(QIcon(":/res/trash.png"), tr("Clear"));
    QAction * result = menu.exec(pt);
    if (result != NULL)
    {
        if (result == edit)
            openTicketWidget(id);
        else if (result == clear)
        {
            ui.tableWidget->clearContents();
            ui.tableWidget->setRowCount(0);
        }
    }
}

void MainWindow::on_tableWidget_itemDoubleClicked(QTableWidgetItem * item)
{
    openTicketWidget(ui.tableWidget->item(item->row(), 0)->text().toInt());
}

void MainWindow::on_actionNewTicket_triggered()
{
    if (findTicketWidget(0) == NULL)
    {
        TicketWidget * tw = createTicketWidget(0);
        ui.tabWidget->setCurrentIndex(ui.tabWidget->addTab(tw, QIcon(":/res/doc.png"), tr("New Ticket")));
        ui.actionNewTicket->setEnabled(false);
    }
}

void MainWindow::on_actionReloadAllGroups_triggered()
{
    timer->stop();
    QTreeWidgetItem * root = ui.treeWidget->invisibleRootItem();
    for (int i = 0; i < root->childCount(); i++)
    {
        QTreeWidgetItem * item = root->child(i);
        if (!item->data(0, Qt::UserRole).toBool())
        {
            item->setDisabled(true);
            pigser->getGroupTickets(item->data(0, Qt::UserRole + 1).toInt());
        }
    }
    if (timer->interval() > 0) timer->start();
}

void MainWindow::on_actionSearchTickets_triggered()
{
    QDialog dlg(this);
    Ui::SearchDialog sui;
    sui.setupUi(&dlg);

    sui.comboBox_What->addItem(QString("CaseID"), QVariant(QString("caseid")));
    sui.comboBox_What->addItem(QString("Hostopian"), QVariant(QString("hostopian")));
    sui.comboBox_What->addItem(QString("Domain"), QVariant(QString("domain")));
    sui.comboBox_What->addItem(QString("Name"), QVariant(QString("name")));
    sui.comboBox_What->addItem(QString("Email Address"), QVariant(QString("email")));
    sui.comboBox_What->addItem(QString("Opened by"), QVariant(QString("otech")));
    sui.comboBox_What->addItem(QString("Assigned to (tech)"), QVariant(QString("atech")));
    sui.comboBox_What->addItem(QString("Assigned to (group)"), QVariant(QString("agroup")));
    sui.comboBox_What->addItem(QString("Closed by"), QVariant(QString("ctech")));
    sui.comboBox_What->addItem(QString("Date opened"), QVariant(QString("odate")));
    sui.comboBox_What->addItem(QString("Date closed"), QVariant(QString("cdate")));
    sui.comboBox_What->addItem(QString("Priority"), QVariant(QString("priority")));
    sui.comboBox_What->addItem(QString("Status"), QVariant(QString("status")));
    sui.comboBox_What->addItem(QString("Problem"), QVariant(QString("problem")));
    sui.comboBox_What->addItem(QString("Notes"), QVariant(QString("notes")));
    sui.comboBox_What->addItem(QString("Resolution"), QVariant(QString("resolution")));
    sui.comboBox_What->addItem(QString("Audit Trail"), QVariant(QString("audit")));

    sui.comboBox_By->addItem(QString("Contains"), QVariant(QString("like")));
    sui.comboBox_By->addItem(QString("Equal to"), QVariant(QString("=")));
    sui.comboBox_By->addItem(QString("Not equal to"), QVariant(QString("<>")));
    sui.comboBox_By->addItem(QString("Greater than"), QVariant(QString(">")));
    sui.comboBox_By->addItem(QString("Less than"), QVariant(QString("<")));

    sui.comboBox_Status->addItem(QString("Open"), QVariant(QString("open")));
    sui.comboBox_Status->addItem(QString("In Progress"), QVariant(QString("in_progress")));
    sui.comboBox_Status->addItem(QString("Closed"), QVariant(QString("closed")));

    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<QString, QString> map;
        map.insert(QString("what"), sui.comboBox_What->itemData(sui.comboBox_What->currentIndex(), Qt::UserRole).toString());
        map.insert(QString("by"), sui.comboBox_By->itemData(sui.comboBox_By->currentIndex(), Qt::UserRole).toString());
        map.insert(QString("condition"), sui.lineEdit->text());
        if (sui.checkBox->isChecked())
            map.insert(QString("statussearch"), sui.comboBox_Status->itemData(sui.comboBox_Status->currentIndex(), Qt::UserRole).toString());
        ui.actionSearchTickets->setEnabled(false);
        ui.tableWidget->clearContents();
        ui.tableWidget->setRowCount(0);
        ui.tableWidget->setEnabled(false);
        pigser->searchTickets(map);
    }
}

void MainWindow::on_actionReloadTicket_triggered()
{
    TicketWidget * tw = (TicketWidget *)ui.tabWidget->currentWidget();
    if (tw != NULL)
    {
        if (tw->isEnabled() && tw->getID()) {
            ui.actionReloadTicket->setEnabled(false);
            tw->setEnabled(false);
            ui.tabWidget->setTabIcon(ui.tabWidget->indexOf(tw), QIcon(":/res/redo.png"));
            pigser->getTicket(tw->getID());
        }
    }
}

void MainWindow::on_actionCloseTicket_triggered()
{
    int tabI = ui.tabWidget->currentIndex();
    if (tabI != -1)
    {
        ui.tabWidget->removeTab(tabI);
    }

    if (findTicketWidget(0) == NULL)
        ui.actionNewTicket->setEnabled(true);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, tr("Help"), QString("Made by =[igoriok]="));
}

void MainWindow::on_actionAboutQt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

void MainWindow::clearStatusBar()
{
    ui.statusBar->showMessage(QString());
}

void MainWindow::on_tray_activated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        if(this->isVisible()) {
            if(this->isActiveWindow()) {
                this->hide();
            } else this->activateWindow();
        } else {
            this->show();
        }
        break;
    case QSystemTrayIcon::MiddleClick:
        break;
    default:
        break;
    }
}

void MainWindow::showTrayMessage()
{
    if (trayMessages.size() > 0)
    {
        QPair<QString, QString> mess = trayMessages.dequeue();
        tray->showMessage(mess.first, mess.second, QSystemTrayIcon::Information, 3000);
        if (trayMessages.size() > 0)
            QTimer::singleShot(3000, this, SLOT(showTrayMessage()));
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    if((e->type() == QEvent::WindowStateChange) && (this->isMinimized()) && settings->showTrayIcon()) {
        this->hide();
    }
}

MainWindow::~MainWindow()
{
    settings->setMainWindowState(this->saveState());
    settings->setLang(ui.actionRussian->isChecked());
    QList<int> gr;
    QTreeWidgetItem * root = ui.treeWidget->invisibleRootItem();
    for (int i = 0; i < root->childCount(); ++i)
        gr.append(root->child(i)->data(0, Qt::UserRole + 1).toInt());
    settings->setPigsGroups(gr);
    settings->saveConfig();
}

void MainWindow::on_actionSettings_triggered()
{
    settings->showDialog();
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if (index != -1)
        ui.tabWidget->removeTab(index);
    if (findTicketWidget(0) == NULL)
        ui.actionNewTicket->setEnabled(true);
}

void MainWindow::on_actionEnglish_toggled(bool checked)
{
    if (checked)
    {
        ui.actionRussian->setChecked(false);
        QApplication::removeTranslator(appTranslator);
        ui.retranslateUi(this);
    }
}

void MainWindow::on_actionRussian_toggled(bool checked)
{
    if (checked)
    {
        ui.actionEnglish->setChecked(false);
        QApplication::installTranslator(appTranslator);
        ui.retranslateUi(this);
    }
}

void MainWindow::on_dockWidget_Cases_dockLocationChanged(Qt::DockWidgetArea area)
{
}

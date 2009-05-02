#include <QtGui/QApplication>
#include <QtCore/QTranslator>
#include "mainwindow.h"
#include "Ticket.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName("PigsON");
    MainWindow w;
    w.show();
    return a.exec();
}

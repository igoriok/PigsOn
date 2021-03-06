# -------------------------------------------------
# Project created by QtCreator 2009-02-21T14:12:35
# -------------------------------------------------
QT += network \
    webkit
TARGET = PigsON_Direct
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    TicketWidget.cpp \
    TicketInfo.cpp \
    Ticket.cpp \
    HostoperButton.cpp \
    SettingsDialog.cpp \
    SettingsManager.cpp \
    HtmlPigsClient.cpp \
    WebKitPigsClient.cpp \
    WebKitPigsParser.cpp
HEADERS += mainwindow.h \
    TicketWidget.h \
    TicketInfo.h \
    Ticket.h \
    SAccount.h \
    HostoperButton.h \
    SettingsDialog.h \
    SettingsManager.h \
    HtmlPigsClient.h \
    PigsClient.h \
    WebKitPigsClient.h \
    WebKitPigsParser.h
FORMS += mainwindow.ui \
    TicketWidget.ui \
    SettingsDialog.ui \
    SearchDialog.ui
OTHER_FILES += Classes.xml
RESOURCES += Resources.qrc
TRANSLATIONS = ru.ts

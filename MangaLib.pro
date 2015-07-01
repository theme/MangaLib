#-------------------------------------------------
#
# Project created by QtCreator 2015-06-23T11:54:50
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MangaLib
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    filestableview.cpp \
    hashthread.cpp \
    dbtablewidget.cpp \
    dbschema.cpp \
    fileexplorer.cpp \
    fileinfowidget.cpp \
    sqlitedb.cpp \
    lrline.cpp

HEADERS  += mainwindow.h \
    filestableview.h \
    hashthread.h \
    dbtablewidget.h \
    dbschema.h \
    fileexplorer.h \
    fileinfowidget.h \
    sqlitedb.h \
    lrline.h

FORMS    += mainwindow.ui \
    dbtablewidget.ui \
    fileexplorer.ui \
    fileinfowidget.ui \
    lrline.ui

RESOURCES += \
    icon.qrc

OTHER_FILES += \
    icons/icon.svg \
    dbschema.json

#CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

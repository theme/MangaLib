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
    hashthread.cpp \
    dbtablewidget.cpp \
    dbschema.cpp \
    fileexplorer.cpp \
    sqlitedb.cpp \
    fsmixdbmodel.cpp \
    hashpool.cpp \
    tagpool.cpp \
    tagtypeselector.cpp \
    filetagswidget.cpp \
    dbmanager.cpp \
    rankpool.cpp \
    rankdelegate.cpp \
    rank/stareditor.cpp \
    rank/starrating.cpp \
    fsfilterproxymodel.cpp

HEADERS  += mainwindow.h \
    hashthread.h \
    dbtablewidget.h \
    dbschema.h \
    fileexplorer.h \
    sqlitedb.h \
    fsmixdbmodel.h \
    hashpool.h \
    tagpool.h \
    tagtypeselector.h \
    filetagswidget.h \
    dbmanager.h \
    rankpool.h \
    rankdelegate.h \
    rank/stareditor.h \
    rank/starrating.h \
    fsfilterproxymodel.h

FORMS    += mainwindow.ui \
    dbtablewidget.ui \
    fileexplorer.ui \
    tagtypeselector.ui \
    filetagswidget.ui \
    dbmanager.ui

RESOURCES += \
    icon.qrc

OTHER_FILES += \
    icons/icon.svg \
    dbschema.json

#CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

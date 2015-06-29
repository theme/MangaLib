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
    sqltablewidget.cpp \
    filestableview.cpp \
    hashthread.cpp

HEADERS  += mainwindow.h \
    sqltablewidget.h \
    filestableview.h \
    hashthread.h

FORMS    += mainwindow.ui

RESOURCES += \
    icon.qrc

OTHER_FILES += \
    icons/icon.svg

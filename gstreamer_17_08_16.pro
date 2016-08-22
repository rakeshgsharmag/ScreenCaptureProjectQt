#-------------------------------------------------
#
# Project created by QtCreator 2016-08-17T10:27:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gstreamer_17_08_16
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

#INCLUDEPATH += C:\0.10\x86_64\include\gstreamer-0.10 \
#               C:\0.10\x86_64\include\glib-2.0 \
#               C:\0.10\x86_64\lib\x86_64-linux-gnu\glib-2.0\include \
#               C:\0.10\x86_64\lib\glib-2.0\include \
#               C:\0.10\x86_64\include\libxml2

#LIBPATH += C:\0.10\x86_64\lib

#LIBS += -LC:\0.10\x86_64\lib\ \
#        -lgstreamer-0.10 \
#        -lgobject-2.0 \
#        -lglib-2.0

INCLUDEPATH += D:\gstreamer-sdk\0.10\x86_64\include\gstreamer-0.10 \
               D:\gstreamer-sdk\0.10\x86_64\include\glib-2.0 \
               D:\gstreamer-sdk\0.10\x86_64\lib\x86_64-linux-gnu\glib-2.0\include \
               D:\gstreamer-sdk\0.10\x86_64\lib\glib-2.0\include \
               D:\gstreamer-sdk\0.10\x86_64\include\libxml2

LIBPATH += C:\0.10\x86_64\lib

LIBS += -LD:\gstreamer-sdk\0.10\x86_64\lib\ \
        -lgstreamer-0.10 \
        -lgobject-2.0 \
        -lglib-2.0

RESOURCES += \
    logo.qrc


#-------------------------------------------------
#
# Project created by QtCreator 2016-12-28T20:17:48
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FlightManagerSystem
TEMPLATE = app
INCLUDEPATH +=  . \
                ./Connect/ \
                ./Login/ \
                ./MainWindow

SOURCES +=  main.cpp \
            Login/login.cpp \
            Connect/connectdatabase.cpp \
            MainWindow/flightmanager.cpp

HEADERS +=  Login/login.h \
            Connect/connectdatabase.h \
            MainWindow/flightmanager.h

FORMS    += $$PWD/Login/login.ui \
    MainWindow/flightmanager.ui

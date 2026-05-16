QT += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = CurrencyConverter
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/apifetcher.cpp

HEADERS += \
    src/mainwindow.h \
    src/apifetcher.h \
    libs/json.hpp

INCLUDEPATH += src/ libs/

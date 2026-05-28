QT       += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TEMPLATE = app
TARGET   = CurrencyConverter

CONFIG += warn_on

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/apifetcher.cpp

HEADERS += \
    src/basefetcher.h \
    src/apifetcher.h \
    src/mainwindow.h \
    src/currencyconverter.h \
    src/conversionrecord.h \
    src/historymanager.h

INCLUDEPATH += src libs

# Place build output in build/
DESTDIR     = build
OBJECTS_DIR = build/.obj
MOC_DIR     = build/.moc
RCC_DIR     = build/.rcc
UI_DIR      = build/.ui

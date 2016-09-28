#-------------------------------------------------
#
# Project created by QtCreator 2016-09-26T16:00:54
#
#-------------------------------------------------

QT       += core gui network widgets

TARGET = crash-reporter
TEMPLATE = app


SOURCES += src/main.cpp \
    src/CrashReporterDialog.cpp

HEADERS  += \
    src/CrashReporterDialog.h

FORMS    += \
    src/CrashReporterDialog.ui

RESOURCES += resources/resources.qrc

TRANSLATIONS += resources/translations/crash-reporter.ts

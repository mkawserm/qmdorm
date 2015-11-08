#-------------------------------------------------
#
# Project created by QtCreator 2015-11-08T03:09:40
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = qmdorm
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    userinformation.cpp

include(qmdorm.pri)

HEADERS += \
    userinformation.hpp

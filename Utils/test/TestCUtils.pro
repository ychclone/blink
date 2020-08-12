#QMAKE_CC = gcc
QMAKE_CXX = g++

TEMPLATE    = app

QT -= gui

CONFIG += console warn_on qtestlib
INCLUDEPATH += "../"

SOURCES     += TestCUtils.cpp \
			   ../CUtils.cpp \

HEADERS     += TestCUtils.h \
			   ../CUtils.h \


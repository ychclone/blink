#QMAKE_CC = gcc
QMAKE_CXX = g++

#TEMPLATE    = app

QT -= gui

CONFIG += console warn_on qtestlib
INCLUDEPATH += "../" "../../"

SOURCES     += TestCRecordPool.cpp \
			   ../CRecordPool.cpp \
			   ../../CUtils.cpp \

HEADERS     += TestCRecordPool.h \
			   ../CRecordPool.h \
			   ../../CUtils.h \



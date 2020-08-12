#QMAKE_CC = gcc
QMAKE_CXX = g++

TEMPLATE    = app

#QT = core
QT -= gui

CONFIG += console warn_on
LIBS += -L"../external"
INCLUDEPATH += "../"
INCLUDEPATH += "../../Model/qTagger/" 

SOURCES     += qTag.cpp \
			   qTagApp.cpp \
			   ../../Model/qTagger/CTagResultItem.cpp \
			   ../../Model/qTagger/qTagger.cpp \
			   ../../Model/COutputItem.cpp \
			   ../../Model/qTagger/CSourceFileList.cpp \
			   ../CUtils.cpp \

HEADERS     += qTagApp.h \
			   ../../Model/qTagger/CTagResultItem.h \
			   ../../Model/qTagger/qTagger.h \
			   ../../Model/COutputItem.h \
			   ../../Model/qTagger/CSourceFileList.h \
			   ../CUtils.h \



TEMPLATE     = lib
CONFIG      += plugin
#LIBS       +=  -ldb_cxx -ldb -lgdbm
LIBS        +=  -ldb47
INCLUDEPATH += ../external
#DESTDIR     = ../plugins
HEADERS      = dbmplugin.h
SOURCES      = dbmplugin.cpp


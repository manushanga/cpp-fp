TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    parser.cpp \
    codemodel.cpp \
    filereader.cpp \
    worker.cpp

HEADERS += \
    parser.h \
    codemodel.h \
    filereader.h \
    worker.h


LIBS += \
    -lboost_system \
    -lboost_filesystem \
    -lboost_iostreams \
    -pthread

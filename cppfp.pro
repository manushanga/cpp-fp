TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    parser.cpp \
    codemodel.cpp \
    filereader.cpp \
    worker.cpp \
    test/test1.cpp \
    searcher.cpp \
    py-exposer.cpp

HEADERS += \
    parser.h \
    codemodel.h \
    filereader.h \
    worker.h \
    searcher.h \
    common.h


LIBS += \
    -lboost_system \
    -lboost_filesystem \
    -lboost_iostreams \
    -pthread

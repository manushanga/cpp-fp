TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    parser.cpp \
    codemodel.cpp

HEADERS += \
    parser.h \
    codemodel.h

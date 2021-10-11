QT += testlib network concurrent
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

@
win32:LIBS += -luser32
win32:LIBS += -lpsapi
win32:LIBS += -lkernel32
@

include($$PWD/../lib/QConsoleListener/src/qconsolelistener.pri)

SOURCES += \
    tst_answerbot.cpp \
    ../AnswerBotSrc/answerer.cpp \
    ../AnswerBotSrc/networkmanager.cpp \
    ../AnswerBotSrc/reader.cpp \
    ../AnswerBotSrc/tinyexpr.c

HEADERS += \
    ../AnswerBotSrc/answerer.h \
    ../AnswerBotSrc/networkmanager.h \
    ../AnswerBotSrc/reader.h \
    ../AnswerBotSrc/structs.h \
    ../AnswerBotSrc/tinyexpr.h

INCLUDEPATH += ../AnswerBotSrc

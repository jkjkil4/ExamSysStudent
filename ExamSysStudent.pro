#-------------------------------------------------
#
# Project created by QtCreator 2022-02-15T22:18:37
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++

TARGET = ExamSysStudent
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        Ques/ques.cpp \
        Ques/queschoice.cpp \
        Ques/quesinvalid.cpp \
        Ques/quesmultichoice.cpp \
        Ques/quessinglechoice.cpp \
        Ques/queswhether.cpp \
        SubWidget/examview.cpp \
        SubWidget/loginview.cpp \
        Widget/doubleslidebutton.cpp \
        Widget/scorewidget.cpp \
        main.cpp \
        widget.cpp

HEADERS += \
        Ques/ques.h \
        Ques/queschoice.h \
        Ques/quesinvalid.h \
        Ques/quesmultichoice.h \
        Ques/quessinglechoice.h \
        Ques/queswhether.h \
        SubWidget/examview.h \
        SubWidget/loginview.h \
        Widget/doubleslidebutton.h \
        Widget/scorewidget.h \
        widget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    SubWidget/examview.ui \
    SubWidget/loginview.ui

RESOURCES += \
    src.qrc

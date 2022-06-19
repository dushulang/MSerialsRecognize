#-------------------------------------------------
#
# Project created by QtCreator 2019-08-11T22:08:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MSerialsRecognize
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
        caffe.cpp \
        cryptdialog.cpp \
        formblackside.cpp \
        formrecognize.cpp \
        halcontocv.cpp \
        imagedeal.cpp \
        machine.cpp \
        main.cpp \
        mainwindow.cpp \
        mcamera.cpp

HEADERS += \
        MSerialsCV.h \
        MSerialsCam.h \
        caffe.h \
        cryptdialog.h \
        excv.h \
        formblackside.h \
        formrecognize.h \
        global.h \
        halcontocv.h \
        imagedeal.h \
        machine.h \
        mainwindow.h \
        mcamera.h \
        mserialssystem.h \
        preference.h \
        swaplowhigh.h \
        vardefine.h

FORMS += \
        cryptdialog.ui \
        formblackside.ui \
        formrecognize.ui \
        mainwindow.ui

INCLUDEPATH +=  ../third_party/opencv4.0.1/include \
                ../third_party/opencv4.0.1/include/opencv2 \
                ../third_party/halcon18/include \
                ../third_party/halcon18/include/halconcpp

#CONFIG += console
QT += network

QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

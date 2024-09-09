QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    buttongroup.cpp \
    common.cpp \
    des.c \
    downloadlayout.cpp \
    downloadtask.cpp \
    downloadwidget.cpp \
    filedataprogress.cpp \
    filepropertyinfodialog.cpp \
    logindialog.cpp \
    logininfoinstance.cpp \
    main.cpp \
    mainwindow.cpp \
    myfilewidget.cpp \
    mymenu.cpp \
    networkdata.cpp \
    sharewidget.cpp \
    titlewidget.cpp \
    transformwidget.cpp \
    uploadlayout.cpp \
    uploadtask.cpp

HEADERS += \
    buttongroup.h \
    common.h \
    des.h \
    downloadlayout.h \
    downloadtask.h \
    downloadwidget.h \
    filedataprogress.h \
    fileinfo.h \
    filepropertyinfodialog.h \
    logindialog.h \
    logininfoinstance.h \
    mainwindow.h \
    myfilewidget.h \
    mymenu.h \
    networkdata.h \
    sharewidget.h \
    titlewidget.h \
    transformwidget.h \
    uploadlayout.h \
    uploadtask.h

FORMS += \
    buttongroup.ui \
    downloadwidget.ui \
    filedataprogress.ui \
    filepropertyinfodialog.ui \
    logindialog.ui \
    mainwindow.ui \
    myfilewidget.ui \
    sharewidget.ui \
    titlewidget.ui \
    transformwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc

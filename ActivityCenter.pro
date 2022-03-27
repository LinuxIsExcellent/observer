QT       += core gui
QT       += network

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
    addfieldlinkdialog.cpp \
    annonationeditwidget.cpp \
    comboboxlineedit.cpp \
    globalapplication.cpp \
    globalconfig.cpp \
    log.cpp \
    logindialog.cpp \
    lualistdatawidget.cpp \
    main.cpp \
    modifcommand.cpp \
    modifyservertimewidget.cpp \
    msg.pb.cc \
    parseconfig.cpp \
    qloadingwidget.cpp \
    selectrowdatadialog.cpp \
    showmsgdialog.cpp \
    stringtotableview.cpp \
    tabledelegate.cpp \
    tabwidgetcell.cpp \
    luatabledatawidget.cpp \
    mainwindow.cpp

HEADERS += \
    Packet.h \
    addfieldlinkdialog.h \
    annonationeditwidget.h \
    comboboxlineedit.h \
    globalapplication.h \
    globalconfig.h \
    log.h \
    logindialog.h \
    lualistdatawidget.h \
    modifcommand.h \
    modifyservertimewidget.h \
    msg.pb.h \
    parseconfig.h \
    qloadingwidget.h \
    selectrowdatadialog.h \
    showmsgdialog.h \
    stringtotableview.h \
    tabledelegate.h \
    tabwidgetcell.h \
    luatabledatawidget.h \
    mainwindow.h

FORMS += \
    addfieldlinkdialog.ui \
    annonationeditwidget.ui \
    comboboxlineedit.ui \
    logindialog.ui \
    modifyservertimewidget.ui \
    qloadingwidget.ui \
    selectrowdatadialog.ui \
    showmsgdialog.ui \
    stringtotableview.ui \
    tabwidgetcell.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix|win32: LIBS += -LE:/workspace/observer/protobuf-master/build_test/ -lprotobuf

INCLUDEPATH += E:/workspace/observer/protobuf-master/src
DEPENDPATH += E:/workspace/observer/protobuf-master/src

unix|win32: LIBS += -LE:/workspace/observer/compileLua/ -lcompileLua

INCLUDEPATH += E:/workspace/observer/compileLua/src
DEPENDPATH += E:/workspace/observer/compileLua/src

RESOURCES += \
    res.qrc

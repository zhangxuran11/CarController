#-------------------------------------------------
#
# Project created by QtCreator 2011-05-11T13:56:34
#
#-------------------------------------------------

QT       += core gui
QMAKE_LFLAGS += -unwind -g -ldl -rdynamic
TARGET = CarController
TEMPLATE = app
include(BroadcastControllerPkg/BroadcastControllerPkg.pri)
include(GPIOPkg/GPIOPkg.pri)
include(GPS_Panel/GPS_Panel.pri)
include(MediaPkg/MediaPkg.pri)
include(ModbusManager/ModbusManager.pri)
include(qextserialport/src/qextserialport.pri)
include(ZDaoPkg/ZDaoPkg.pri)
include(ZDebugPkg/ZDebugPkg.pri)
include(ZTPManager/ZTPManager.pri)
include(ZTools/ZTools.pri)

#DEFINES += ARM
#DEFINES += X86
DEFINES += $$(ARCH)
contains( DEFINES, arm ) {

    DEFINES += ARM
}
!contains( DEFINES, arm ) {
    DEFINES += X86
}
contains( DEFINES, ARM ) {

    LIBS    +=-lgstreamer-0.10
    LIBS +=/opt/poky/qtx11-1.7/sysroots/armv7a-vfp-neon-poky-linux-gnueabi/usr/lib/libgstinterfaces-0.10.so

    LIBS +=/opt/poky/qtx11-1.7/sysroots/armv7a-vfp-neon-poky-linux-gnueabi/usr/lib/libgobject-2.0.so.0
    LIBS +=/opt/poky/qtx11-1.7/sysroots/armv7a-vfp-neon-poky-linux-gnueabi/usr/lib/libglib-2.0.so.0
    INCLUDEPATH +=/opt/poky/qtx11-1.7/sysroots/armv7a-vfp-neon-poky-linux-gnueabi/usr/include/glib-2.0/ \
                 /opt/poky/qtx11-1.7/sysroots/armv7a-vfp-neon-poky-linux-gnueabi/usr/include/glib-2.0/glib/ \
                /opt/poky/qtx11-1.7/sysroots/armv7a-vfp-neon-poky-linux-gnueabi/usr/include/gstreamer-0.10/ \
                /opt/poky/qtx11-1.7/sysroots/armv7a-vfp-neon-poky-linux-gnueabi/usr/lib/glib-2.0/include/ \
                /opt/poky/qtx11-1.7/sysroots/armv7a-vfp-neon-poky-linux-gnueabi/usr/include/libxml2/ \
                /opt/poky/qtx11-1.7/sysroots/armv7a-vfp-neon-poky-linux-gnueabi/usr/lib/i386-linux-gnu/glib-2.0/include/ \
                /usr/include/qt4/

    mytarget.commands += scp ./${TARGET} root@192.168.13.10:/appbin/
    mytarget.commands += && scp ./${TARGET} root@192.168.12.10:/appbin/
}
contains( DEFINES, X86 ) {

    LIBS    +=-lgstreamer-0.10 -lgstreamermm-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lxml2 -lglib-2.0
    LIBS +=/usr/lib/i386-linux-gnu/libgstinterfaces-0.10.so

    LIBS +=/usr/lib/i386-linux-gnu/libgobject-2.0.so.0
    LIBS +=/usr/lib/vmware-tools/lib32/libglib-2.0.so.0/libglib-2.0.so.0
    INCLUDEPATH +=/usr/include/glib-2.0/ \
                 /usr/include/glib-2.0/glib/ \
                /usr/include/gstreamer-0.10/ \
                /usr/lib/i386-linux-gnu/glib-2.0/include/ \
                /usr/include/libxml2/ \
                /usr/lib/i386-linux-gnu/glib-2.0/include/ \
                /usr/lib/gstreamermm-0.10/include/
}
SOURCES += main.cpp\
        CQplayerGUI.cpp \
    workthread.cpp \
    globalinfo.cpp \
    iodevicelistener.cpp \
    zutility.cpp \
    softwatchdog.cpp \
    crashtool.cpp \
    versionsender.cpp \
    vserverctrl.cpp \
    mytranslator.cpp

HEADERS  += CQplayerGUI.h \
    config.h \
    workthread.h \
    globalinfo.h \
    iodevicelistener.h \
    zutility.h \
    softwatchdog.h \
    crashtool.h \
    versionsender.h \
    vserverctrl.h \
    mytranslator.h

FORMS    += CQplayerGUI.ui



RESOURCES += \
    resource.qrc
mytarget.target = a

mytarget.depends =



QMAKE_EXTRA_TARGETS += mytarget

OTHER_FILES += \
    channelSet.ini \
    CarController.pro.user

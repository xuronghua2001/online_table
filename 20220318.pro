QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
RC_ICONS = image.ico
CONFIG += c++17
CONFIG += thread exceptions rtti stl
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
macx: LIBS += -framework AppKit

CONFIG +=  warn_on

include($$PWD/3rdparty/qBreakpad/qBreakpad.pri)

SOURCES += \
    Multithreading.cpp \
    main.cpp \
    widget.cpp

HEADERS += \
    Multithreading.h \
    qbreakpadlib/QBreakpadHandler.h \
    qbreakpadlib/QBreakpadHttpUploader.h \
    qbreakpadlib/singletone/call_once.h \
    qbreakpadlib/singletone/singleton.h \
    widget.h

FORMS += \
    widget.ui
    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

DISTFILES += \
    qbreakpadlib/lib/release/qBreakpad.lib

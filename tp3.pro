INCLUDEPATH += $$PWD
QT       += network
SOURCES += $$PWD/openglwindow.cpp \
    tcpserver.cpp \
    trianglewindow.cpp \
    camera.cpp \
    point.cpp \
    particule.cpp
HEADERS += $$PWD/openglwindow.h \
    tcpserver.h \
    trianglewindow.h \
    camera.h \
    point.h \
    particule.h

SOURCES += \
    main.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
INSTALLS += target
CONFIG   += console
QMAKE_MAC_SDK = macosx10.10

RESOURCES += \
    gestionnaire.qrc

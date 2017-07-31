QT       += core gui opengl multimedia sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = FotoYa
TEMPLATE = app

win32:LIBS += -lopengl32
win32:LIBS += -lglu32
unix:LIBS += -lGLU

#DEFINES += DEBUG_O1

SOURCES += main.cpp \
    adminDB.cpp \
    publicador.cpp \
    confevento.cpp \
    configuracionglobal.cpp \
    galeria.cpp \
    impresor.cpp \
    login.cpp \
    manager.cpp \
    proyectador.cpp \
    scene.cpp \
    search.cpp

HEADERS  += \
    adminDB.h \
    publicador.h \
    confevento.h \
    configuracionglobal.h \
    galeria.h \
    impresor.h \
    login.h \
    manager.h \
    proyectador.h \
    scene.h \
    search.h \
    texture.h \
    video.h

FORMS += \
    publicador.ui \
    confevento.ui \
    configuracionglobal.ui \
    galeria.ui \
    impresor.ui \
    login.ui \
    proyectador.ui \
    search.ui

RESOURCES += \
    recursos.qrc

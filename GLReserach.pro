#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T17:22:57
#
#-------------------------------------------------

CONFIG += c++11

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GLReserach
TEMPLATE = app


SOURCES += main.cpp\
        glwindow.cpp \
    glscene.cpp \
    glbatch.cpp \
    glshadermanager.cpp \
    glframe.cpp

HEADERS  += glwindow.h \
    glscene.h \
    glbatch.h \
    glbatchbase.h \
    glbase.h \
    glshadermanager.h \
    platformdefinition.h \
    shadersourcefile.h \
    glframe.h \
    GameSrc/BlockBase.h \
    GameSrc/BlockDataBatch.h \
    GameSrc/BlockManager.h \
    GameSrc/BlockInstance.h \
    GameSrc/Chunk.h \
    GameSrc/ChunkManager.h \
    GameSrc/Skybox.h \
    GameSrc/ChunkUpdateWorker.h \
    GameSrc/WorldGenerator.h

RESOURCES += \
    resources.qrc

DISTFILES += \
    adsphong.vert \
    ISSUES

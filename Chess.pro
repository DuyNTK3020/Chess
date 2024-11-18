#-------------------------------------------------
#
# Project created by QtCreator 2017-04-25T17:19:24
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Chess
TEMPLATE = app


SOURCES += main.cpp \
    clientmanager.cpp \
    game.cpp \
    chessbox.cpp \
    chessboard.cpp \
    chesspiece.cpp \
    player.cpp \
    queen.cpp \
    pawn.cpp \
    bishop.cpp \
    knight.cpp \
    rook.cpp \
    king.cpp \
    button.cpp \
    user.cpp

HEADERS  += \
    clientmanager.h \
    game.h \
    chessbox.h \
    chessboard.h \
    chesspiece.h \
    player.h \
    queen.h \
    pawn.h \
    bishop.h \
    knight.h \
    rook.h \
    king.h \
    button.h \
    user.h

FORMS    +=

RESOURCES += \
    resource.qrc

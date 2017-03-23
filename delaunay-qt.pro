TEMPLATE = app
TARGET = name_of_the_app

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    main.cpp \
    face_tree.cpp \
    geometry.cpp \
    linearLeastSquares.c

HEADERS += \
    linearLeastSquares.h \
    geometry.hpp \
    face_tree.h

CONFIG += c++11 \
    lstdc++

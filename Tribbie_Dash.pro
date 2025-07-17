QT += core gui widgets multimedia multimediawidgets  # 添加多媒体模块
CONFIG += c++17

SOURCES += \
    collisionmanager.cpp \
    gameitem.cpp \
    gamewindow.cpp \
    goldcoin.cpp \
    itemmanager.cpp \
    itempatterngenerator.cpp \
    letter.cpp \
    levelselectwindow.cpp \
    lionshield.cpp \
    main.cpp \
    mainwindow.cpp \
    receiver.cpp \
    redcrystal.cpp \
    resultwindow.cpp \
    speedpig.cpp \
    videowindow.cpp

HEADERS += \
    collisionmanager.h \
    gameitem.h \
    gamewindow.h \
    goldcoin.h \
    itemmanager.h \
    itempatterngenerator.h \
    letter.h \
    levelselectwindow.h \
    lionshield.h \
    mainwindow.h \
    receiver.h \
    redcrystal.h \
    resultwindow.h \
    speedpig.h \
    videowindow.h

FORMS +=

RESOURCES += \
    resources.qrc

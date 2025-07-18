QT += core gui widgets multimedia multimediawidgets  # 添加多媒体模块
CONFIG += c++17

SOURCES += \
    character.cpp \
    collisionmanager.cpp \
    gameitem.cpp \
    gamewindow.cpp \
    goldcoin.cpp \
    itemmanager.cpp \
    itempatterngenerator.cpp \
    letter.cpp \
    levelselectwindow.cpp \
    lionshield.cpp \
    magnet.cpp \
    main.cpp \
    mainwindow.cpp \
    receiver.cpp \
    redcrystal.cpp \
    resultwindow.cpp \
    spear.cpp \
    videowindow.cpp

HEADERS += \
    character.h \
    collisionmanager.h \
    gameitem.h \
    gamewindow.h \
    goldcoin.h \
    itemmanager.h \
    itempatterngenerator.h \
    letter.h \
    levelselectwindow.h \
    lionshield.h \
    magnet.h \
    mainwindow.h \
    receiver.h \
    redcrystal.h \
    resultwindow.h \
    spear.h \
    videowindow.h

FORMS +=

RESOURCES += \
    resource.qrc

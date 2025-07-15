#pragma once
#include "gameitem.h"
#include <QSoundEffect>

class RedCrystal : public GameItem {
    Q_OBJECT
public:
    explicit RedCrystal(QWidget* parent, Track track);

public slots:
    void onCollideWithPlayer();
};

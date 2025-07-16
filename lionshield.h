#pragma once
#include "gameitem.h"

class LionShield : public GameItem {
    Q_OBJECT
public:
    explicit LionShield(QWidget* parent, Track track);

public slots:
    void onCollideWithPlayer();
};

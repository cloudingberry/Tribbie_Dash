#pragma once
#include "gameitem.h"

class Spear : public GameItem {
    Q_OBJECT
public:
    explicit Spear(QWidget* parent, Track track);

public slots:
    void onCollideWithPlayer();

};

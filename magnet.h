#pragma once
#include "gameitem.h"

class Magnet : public GameItem {
    Q_OBJECT
public:
    explicit Magnet(QWidget* parent, Track track);

public slots:
    void onCollideWithPlayer();

};

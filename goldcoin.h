#pragma once
#include "gameitem.h"

class GoldCoin : public GameItem {
    Q_OBJECT
public:
    explicit GoldCoin(QWidget* parent, Track track);

public slots:
    void onCollideWithPlayer();

};

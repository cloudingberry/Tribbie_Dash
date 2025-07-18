#pragma once
#include "gameitem.h"

class SpeedPig : public GameItem {
    Q_OBJECT
public:
    explicit SpeedPig(QWidget* parent, Track track);

public slots:
    void onCollideWithPlayer() override;
};

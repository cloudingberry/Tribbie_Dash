#pragma once
#include "gameitem.h"

class Letter : public GameItem {
    Q_OBJECT
public:
    explicit Letter(QWidget* parent, Track track);

public slots:
    void onCollideWithPlayer();
};

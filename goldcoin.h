#pragma once
#include "gameitem.h"

class GoldCoin : public GameItem {
    Q_OBJECT
public:
    explicit GoldCoin(QWidget* parent, Track track);

    void updatePosition(qreal delta) override;

public slots:
    void onCollideWithPlayer() ;

};

#pragma once
#include "gameitem.h"
#include <QPropertyAnimation>
#include <QSoundEffect>

class GoldCoin : public GameItem {
    Q_OBJECT
public:
    explicit GoldCoin(QWidget* parent, Track track);

public slots:
    void onCollideWithPlayer();

private:
    QSoundEffect* m_sound = nullptr;
};

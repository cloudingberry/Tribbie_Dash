#pragma once
#include "gameitem.h"
#include <QSoundEffect>

class LionShield : public GameItem {
    Q_OBJECT
public:
    explicit LionShield(QWidget* parent, Track track);

public slots:
    void onCollideWithPlayer();  // ← 添加这个声明
};

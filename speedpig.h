#pragma once
#include "gameitem.h"

class SpeedPig : public GameItem {
    Q_OBJECT
public:
    explicit SpeedPig(QWidget* parent, Track track);

signals:
    // 发送加速信号（持续时间ms，加速倍数）
    void speedBoostActivated(int duration, float multiplier);

public slots:
    void onCollideWithPlayer() override;
};

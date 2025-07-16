#pragma once
#include "gameitem.h"
#include <QSoundEffect>

class SpeedPig : public GameItem
{
    Q_OBJECT
public:
    explicit SpeedPig(QWidget* parent, Track track);

public slots:
    void onCollideWithPlayer() override;

signals:
    void speedBoost(qreal factor);   // 立即加速
    void speedRestore();             // 3 秒后恢复

private:
    static constexpr int BOOST_DURATION = 3000; // ms
    QSoundEffect* m_sound = nullptr;
};

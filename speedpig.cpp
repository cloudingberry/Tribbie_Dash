#include "speedpig.h"
#include <QSoundEffect>

#include "speedpig.h"

SpeedPig::SpeedPig(QWidget* parent, Track track)
    : GameItem(parent, ":/images/speedpig.PNG", track, QSize(90, 90)) {
    // 设置拾取音效
    setSound("qrc:/sound/coin.wav", 1.0);
}

void SpeedPig::onCollideWithPlayer() {
    stop(); // 停止移动
    m_soundEffect->play(); // 播放音效
    emit speedBoostActivated(5000, 5); // 5秒加速，5倍速度
    fadeAndDelete(); // 消失动画
}

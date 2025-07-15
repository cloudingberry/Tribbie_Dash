#include "redcrystal.h"
#include <QGraphicsOpacityEffect>

RedCrystal::RedCrystal(QWidget* parent, Track track)
    : GameItem(parent, ":/images/redcrystal.png", track, QSize(80, 80))
{
    // 设置透明度效果供动画使用
    auto* effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(1.0);  // 出场时完全不透明
    setGraphicsEffect(effect);
}

void RedCrystal::onCollideWithPlayer() {
    setActive(false);

    QSoundEffect* sound = new QSoundEffect(this);
    sound->setSource(QUrl("qrc:/sound/hurt.WAV"));
    sound->setVolume(1.0);
    sound->play();

    fadeAndDelete(); // 通用淡出动画
}

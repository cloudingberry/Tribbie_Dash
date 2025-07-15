#include "letter.h"

Letter::Letter(QWidget* parent, Track track)
    : GameItem(parent, ":/images/letter.png", track, QSize(80, 80))
{
    // 添加透明度效果（用于 fade 动画）
    auto* effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(1.0);  // 出场时完全不透明
    setGraphicsEffect(effect);

}

void Letter::onCollideWithPlayer() {
    setActive(false);

    QSoundEffect* sound = new QSoundEffect(this);
    sound->setSource(QUrl("qrc:/sound/letter.wav"));
    sound->setVolume(0.9);
    sound->play();

    fadeAndDelete(); // 使用基类封装的淡出删除逻辑
}

#include "goldcoin.h"
#include <QGraphicsOpacityEffect>

GoldCoin::GoldCoin(QWidget* parent, Track track)
    : GameItem(parent, ":/images/coin.png", track, QSize(70, 70)) {

    // 添加透明度效果（必须）
    auto* effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(1.0);  // 出场时完全不透明
    setGraphicsEffect(effect);

    m_sound = new QSoundEffect(this);
    m_sound->setSource(QUrl("qrc:/sound/coin.wav"));
    m_sound->setVolume(0.9);
}

void GoldCoin::onCollideWithPlayer() {
    if (m_sound) m_sound->play();

    fadeAndDelete();
}

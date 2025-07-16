#include "goldcoin.h"

GoldCoin::GoldCoin(QWidget* parent, Track track)
    : GameItem(parent, ":/images/coin.png", track, QSize(70, 70)) {

    setSound("qrc:/sound/coin.wav", 1.0);

}

void GoldCoin::onCollideWithPlayer() {
    stop();

    m_soundEffect->play();

    fadeAndDelete();
}

#include "magnet.h"

Magnet::Magnet(QWidget* parent, Track track)
    : GameItem(parent, ":/images/magnet.png", track, QSize(90, 90)) {

    setSound("qrc:/sound/magnet_pickup.WAV", 1.5);

}

void Magnet::onCollideWithPlayer() {
    stop();

    m_soundEffect->play();

    fadeAndDelete();
}

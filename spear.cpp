#include "spear.h"

Spear::Spear(QWidget* parent, Track track)
    : GameItem(parent, ":/images/spear.png", track, QSize(200, 200)) {

    setSound("qrc:/sound/spear_pickup.WAV", 1.0);

}

void Spear::onCollideWithPlayer() {
    stop();

    m_soundEffect->play();

    fadeAndDelete();
}

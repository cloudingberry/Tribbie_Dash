#include "redcrystal.h"

RedCrystal::RedCrystal(QWidget* parent, Track track)
    : GameItem(parent, ":/images/redcrystal.png", track, QSize(80, 80))
{
    setSound("qrc:/sound/hurt.WAV", 2.0);
}

void RedCrystal::onCollideWithPlayer() {
    stop();

    m_soundEffect->play();

    fadeAndDelete();
}

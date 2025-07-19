#include "letter.h"

Letter::Letter(QWidget* parent, Track track)
    : GameItem(parent, ":/images/letter.png", track, QSize(80, 80))
{
    setSound("qrc:/sound/letter.wav", 2.0);

}

void Letter::onCollideWithPlayer() {
    stop();

    m_soundEffect->play();

    fadeAndDelete();
}

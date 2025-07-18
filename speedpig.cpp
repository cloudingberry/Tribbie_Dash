#include "speedpig.h"

SpeedPig::SpeedPig(QWidget* parent, Track track)
    : GameItem(parent, ":/images/speedpig.png", track, QSize(90, 90)) {
    setSound("qrc:/sound/speedpig.WAV", 1.0);
}

void SpeedPig::onCollideWithPlayer() {
    stop();
    m_soundEffect->play();
    fadeAndDelete();
}

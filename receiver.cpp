#include "receiver.h"

Receiver::Receiver(QWidget* parent, Track track)
    : GameItem(parent, "", track) {
    setImage(":/images/mail.png");
    setScaledSize(QSize(100, 100));
}

void Receiver::onCollideWithPlayer() {
    setActive(false);

    QSoundEffect* sound = new QSoundEffect(this);
    sound->setSource(QUrl("qrc:/sound/mail.wav"));
    sound->setVolume(0.9);
    connect(sound, &QSoundEffect::playingChanged, [=]() {
        if (!sound->isPlaying()) emit goalReached();
    });
    sound->play();
}

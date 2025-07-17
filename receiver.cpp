#include "receiver.h"

Receiver::Receiver(QWidget* parent, Track track)
    : GameItem(parent, "", track)
{
    setImage(":/images/mail.png");
    setScaledSize(QSize(130, 130));
    setSound("qrc:/sound/mail.wav", 1.0);
}

void Receiver::onCollideWithPlayer() {
    stop();

    m_soundEffect->play();

    // 使用基类的 m_soundEffect，监听播放结束
    if (m_soundEffect) {
        connect(m_soundEffect, &QSoundEffect::playingChanged, this, [=]() {
            if (!m_soundEffect->isPlaying()) {
                emit goalReached();
            }
        });
    } else {
        // 保险机制：若播放失败也不影响流程
        emit goalReached();
    }
}

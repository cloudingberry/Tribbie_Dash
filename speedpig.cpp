#include "speedpig.h"
#include <QRandomGenerator>
#include <QSoundEffect>
#include <QTimer>

SpeedPig::SpeedPig(QWidget* parent, Track track)
    : GameItem(parent, "", track, QSize(70, 70))
{
    // 随机外观：两张小猪图
    const QStringList imgs{":/images/speed_pig1.png",
                           ":/images/speed_pig2.png"};
    setImage(imgs[QRandomGenerator::global()->bounded(imgs.size())]);

    // 音效
    m_sound = new QSoundEffect(this);
    m_sound->setSource(QUrl("qrc:/sound/pig_boost.wav"));
    m_sound->setVolume(0.9f);
}

void SpeedPig::onCollideWithPlayer()
{
    // 1. 停掉自己的运动，防止重复碰撞
    stop();

    // 2. 播放音效
    if (m_sound) m_sound->play();

    // 3. 通知场景立即加速
    emit speedBoost(1.5);   // 1.5 倍速

    // 4. 3 秒后恢复原速
    QTimer::singleShot(BOOST_DURATION, this, [this](){
        emit speedRestore();
    });

    // 5. 标记失效，交由 CollisionManager 统一清理
    setActive(false);
}

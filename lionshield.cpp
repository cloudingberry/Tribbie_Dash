#include "lionshield.h"
#include <QSoundEffect>
#include <QRandomGenerator>

LionShield::LionShield(QWidget* parent, Track track)
    : GameItem(parent, "", track)  // 先传空，让后面调用 setImage
{
    // 随机选一张狮子盾牌
    const QStringList images = {
        ":/images/lion_shield1.png",
        ":/images/lion_shield2.png"
    };
    int idx = QRandomGenerator::global()->bounded(images.size());
    setImage(images[idx]);
    setScaledSize(QSize(80, 80));
}

void LionShield::onCollideWithPlayer() {
    stop();

    QSoundEffect* sound = new QSoundEffect(this);
    sound->setSource(QUrl("qrc:/sound/fail.wav"));
    sound->play();

    deleteLater();  // ✅ 或你可以改为添加动画
}

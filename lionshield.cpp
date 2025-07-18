#include "lionshield.h"
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

    setSound("qrc:/sound/fail.wav", 1.0);
}

void LionShield::onCollideWithPlayer() {
    stop();

    deleteLater();
}

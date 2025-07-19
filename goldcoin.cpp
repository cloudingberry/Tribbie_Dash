#include "goldcoin.h"
#include "gamewindow.h"
#include "character.h"

GoldCoin::GoldCoin(QWidget* parent, Track track)
    : GameItem(parent, ":/images/coin.png", track, QSize(70, 70)) {

    setSound("qrc:/sound/coin.wav", 2.0);

}

void GoldCoin::onCollideWithPlayer() {
    stop();

    m_soundEffect->play();

    fadeAndDelete();
}

void GoldCoin::updatePosition(qreal delta) {
    if (GameWindow* gw = qobject_cast<GameWindow*>(parentWidget())) {
        if (Character* c = gw->getCharacter(); c && c->isMagnetActive()) {
            QPointF myCenter = geometry().center();
            QPointF target = c->geometry().center();
            QPointF diff = target - myCenter;

            qreal distance = std::hypot(diff.x(), diff.y());

            // 越近吸得越快（吸力 ∝ 1/distance）
            qreal factor = qBound(0.05, 80.0 / (distance + 40.0), 0.5);

            move(x() + diff.x() * factor, y() + diff.y() * factor);
            return;
        }
    }

    // 默认左移
    GameItem::updatePosition(delta);
}


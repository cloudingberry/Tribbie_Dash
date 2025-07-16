#include "spearitem.h"
#include <QGraphicsOpacityEffect>

spearitem::spearitem(QWidget* parent, Track track)
    : GameItem(parent, ":/images/spear.png", track, QSize(70, 70)),
    m_spearSound(new QSoundEffect(this))
{

    // 添加透明度效果（必须）
    auto* effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(1.0);  // 出场时完全不透明
    setGraphicsEffect(effect);

    m_spearSound = new QSoundEffect(this);
    m_spearSound->setSource(QUrl("qrc:/sound/spear_charge.wav"));
    m_spearSound->setVolume(0.9);
}

void spearitem::onCollideWithPlayer() {
    if (m_spearSound && m_spearSound->isLoaded()) {  // 正确检查有效性
        fadeAndDelete();
    }
}


// 激活冲刺
void spearitem::activateThrust() {
    m_isActive = true;
    m_spearSound->play();

    // 冲刺动画（示例：缩放效果）
    QPropertyAnimation* anim = new QPropertyAnimation(this, "scale");
    anim->setDuration(300);
    anim->setStartValue(1.0);
    anim->setEndValue(1.5);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

bool spearitem::isActive() const {
    return m_isActive;
}

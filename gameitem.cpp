#include "gameitem.h"
#include <QPixmap>
#include <QPropertyAnimation>

GameItem::GameItem(QWidget* parent,
                   const QString& imagePath,
                   Track track,
                   QSize scaledSize,
                   int speed)
    : QLabel(parent),
    m_speed(speed),
    m_scaledSize(scaledSize),
    m_track(track)
{
    setImage(imagePath);
    setScaledSize(scaledSize);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    // 起始 X（屏幕最右）
    int startX = parent->width();
    int itemH  = scaledSize.height();

    int trackY = 0;
    switch (track) {
    case Top:                trackY = 180 - itemH / 2; break;
    case Middle:             trackY = 310 - itemH / 2; break;
    case Bottom:             trackY = 440 - itemH / 2; break;
    case BetweenTopMiddle:   trackY = (180 + 310 - itemH) / 2; break;
    case BetweenMiddleBottom:trackY = (310 + 440 - itemH) / 2; break;
    }
    // 设置透明度效果
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityEffect->setOpacity(1.0);
    setGraphicsEffect(m_opacityEffect);

    setGeometry(startX, trackY, scaledSize.width(), scaledSize.height());
    show();
}

void GameItem::setImage(const QString& imagePath) {
    if (!imagePath.isEmpty()) {
        QPixmap pix(imagePath);
        setPixmap(pix.scaled(m_scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void GameItem::setScaledSize(QSize size) {
    m_scaledSize = size;
    if (!pixmap().isNull())
        setPixmap(pixmap().scaled(m_scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    resize(m_scaledSize);
}

void GameItem::setActive(bool active) { m_isActive = active; }
bool GameItem::isActive() const        { return m_isActive;  }

void GameItem::updatePosition(qreal deltaSec) {
    if (!m_isActive) return;
    int dx = static_cast<int>(m_speed * deltaSec);
    move(x() - dx, y());
    if (x() + width() < 0) deleteLater();   // 出屏幕后销毁
}

void GameItem::fadeAndDelete(int duration) {
    auto* effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(effect);
    effect->setOpacity(1.0);

    auto* fade = new QPropertyAnimation(effect, "opacity", this);
    fade->setDuration(duration);
    fade->setStartValue(1.0);
    fade->setEndValue(0.0);

    connect(fade, &QPropertyAnimation::finished, this, &GameItem::deleteLater);
    fade->start();
}


void GameItem::stop() {
    m_isActive = false;
}

QRect GameItem::getHitBox() const {
    return geometry().adjusted(30, 30, -30, -30);  // 缩小碰撞区域
}

void GameItem::pause() {
    m_isActive = false;
}

void GameItem::resume() {
    m_isActive = true;
}

void GameItem::setSound(const QString& soundPath, qreal volume) {
    if (!m_soundEffect)
        m_soundEffect = new QSoundEffect(this);
    m_soundEffect->setSource(QUrl(soundPath));
    m_soundEffect->setVolume(volume);
}



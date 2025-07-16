#include "gameitem.h"
#include <QPixmap>
#include <QPropertyAnimation>
#include <QApplication>
#include <QRect>
#include <QUrl>
#include <cmath>

GameItem::GameItem(QWidget* parent,
                   const QString& imagePath,
                   Track track,
                   QSize scaledSize,
                   int speed)
    : QLabel(parent)
    , m_speed(speed)
    , m_scaledSize(scaledSize)
    , m_track(track)
    , m_isActive(true)
    , m_opacityEffect(nullptr)
    , m_soundEffect(nullptr)
    , m_isMagnetic(false)
    , m_isBeingAttracted(false)
    , m_targetPosition(0, 0)
    , m_magneticSpeed(300.0f)
    , m_isPaused(false)
{
    setImage(imagePath);

    // 设置透明度效果
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityEffect->setOpacity(1.0);
    setGraphicsEffect(m_opacityEffect);
}

void GameItem::setImage(const QString& imagePath) {
    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        // 直接设置缩放后的图片
        setPixmap(pixmap.scaled(m_scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    resize(m_scaledSize);
}

void GameItem::setScaledSize(QSize size) {
    m_scaledSize = size;
    // 如果已经有pixmap，重新缩放
    if (!pixmap().isNull()) {
        QPixmap currentPixmap = pixmap();
        setPixmap(currentPixmap.scaled(m_scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    resize(m_scaledSize);
}

void GameItem::setActive(bool active) {
    m_isActive = active;
    if (!active) {
        m_isBeingAttracted = false;  // 停止磁力吸引
    }
}

bool GameItem::isActive() const {
    return m_isActive;
}

void GameItem::updatePosition(qreal deltaSec) {
    if (!m_isActive || m_isPaused) return;

    // 如果正在被磁力吸引，优先执行磁力移动
    if (m_isBeingAttracted) {
        moveTowardsTarget(deltaSec);
        return;
    }

    // 原有的正常移动逻辑
    int newX = x() - static_cast<int>(m_speed * deltaSec);
    move(newX, y());
}

void GameItem::stop() {
    m_isActive = false;
    m_isBeingAttracted = false;
}

void GameItem::pause() {
    m_isPaused = true;
}

void GameItem::resume() {
    m_isPaused = false;
}

void GameItem::fadeAndDelete(int duration) {
    m_isActive = false;
    m_isBeingAttracted = false;

    QPropertyAnimation* animation = new QPropertyAnimation(m_opacityEffect, "opacity");
    animation->setDuration(duration);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);

    connect(animation, &QPropertyAnimation::finished, this, &GameItem::deleteLater);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

QRect GameItem::getHitBox() const {
    // 返回缩小20%的碰撞区域
    QRect rect = geometry();
    int shrinkX = rect.width() * 0.1;
    int shrinkY = rect.height() * 0.1;
    return rect.adjusted(shrinkX, shrinkY, -shrinkX, -shrinkY);
}

void GameItem::setSound(const QString& soundPath, qreal volume) {
    if (m_soundEffect) {
        delete m_soundEffect;
    }

    m_soundEffect = new QSoundEffect(this);
    m_soundEffect->setSource(QUrl::fromLocalFile(soundPath));
    m_soundEffect->setVolume(volume);
}

// 磁力移动实现
void GameItem::moveTowardsTarget(qreal deltaSec) {
    if (!m_isBeingAttracted || !m_isActive) return;

    QPointF currentPos = getPosition();
    QPointF direction = m_targetPosition - currentPos;
    float distance = sqrt(direction.x() * direction.x() + direction.y() * direction.y());

    // 检查是否到达目标位置
    if (distance < ARRIVAL_THRESHOLD) {
        // 移动到目标位置
        QPoint targetPoint = m_targetPosition.toPoint();
        move(targetPoint.x() - width() / 2, targetPoint.y() - height() / 2);
        emit reachedTarget();
        return;
    }

    // 归一化方向向量并计算新位置
    direction /= distance;
    QPointF movement = direction * m_magneticSpeed * deltaSec;
    QPointF newPos = currentPos + movement;

    // 应用新位置
    move(static_cast<int>(newPos.x() - width() / 2),
         static_cast<int>(newPos.y() - height() / 2));
}

QPointF GameItem::getPosition() const {
    // 返回物品中心位置
    return QPointF(x() + width() / 2.0, y() + height() / 2.0);
}

void GameItem::setPosition(const QPointF& pos) {
    // 设置物品中心位置
    move(static_cast<int>(pos.x() - width() / 2),
         static_cast<int>(pos.y() - height() / 2));
}

QPointF GameItem::getTrackPosition() const {
    // 根据轨道返回对应的Y坐标 - 你需要根据实际游戏调整这些值
    switch (m_track) {
    case Top: return QPointF(0, 100);
    case Middle: return QPointF(0, 200);
    case Bottom: return QPointF(0, 300);
    case BetweenTopMiddle: return QPointF(0, 150);
    case BetweenMiddleBottom: return QPointF(0, 250);
    default: return QPointF(0, 200);
    }
}

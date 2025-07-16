#include "goldcoin.h"
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <cmath>

Coin::Coin(QWidget* parent, Track track, QSize scaledSize, int speed)
    : GameItem(parent, ":/images/coin.png", track, scaledSize, speed)
    , m_animationStartTime(QDateTime::currentMSecsSinceEpoch())
{
    // 金币默认支持磁力吸引
    setMagnetic(true);

    // 初始化收集音效（使用父类的m_soundEffect成员）
    setSound(":/sounds/coin_collect.wav", 0.6);

    // 绑定游戏主循环更新（避免独立定时器，与游戏帧率同步）
    connect(this, &GameItem::updateRequested, this, [this](qreal deltaSec) {
        Q_UNUSED(deltaSec);
        // 基于时间计算动画状态（避免累积误差）
        qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_animationStartTime;
        m_currentRotation = fmod(elapsed * ROTATION_SPEED / 1000.0, 360.0);

        // 正弦曲线计算缩放（1.0 ~ 1.1之间波动）
        m_currentScale = 1.0 + (MAX_SCALE - 1.0) * sin(2 * M_PI * elapsed / SCALE_CYCLE);

        update(); // 触发重绘
    });
}

void Coin::moveTowardsTarget(qreal deltaSec) {
    // 非活跃或未被吸引时不移动
    if (!isBeingAttracted() || !isActive()) return;

    // 计算当前位置与目标位置（基于父类接口）
    QPointF currentPos = getPosition();
    QPointF targetPos = getTargetPosition();
    QPointF direction = targetPos - currentPos;
    float distance = hypot(direction.x(), direction.y());

    // 距离越近速度越快（金币特有加速逻辑）
    float speedMultiplier = 1.0f;
    if (distance < 150.0f) {
        speedMultiplier = 1.2f + (150.0f - distance) / 150.0f * 1.8f;
    }

    // 到达目标阈值（避免过度接近导致抖动）
    const float ARRIVAL_THRESHOLD = 8.0f;
    if (distance < ARRIVAL_THRESHOLD) {
        emit reachedTarget(); // 通知碰撞管理器已到达目标
        return;
    }

    // 计算移动向量并更新位置
    QPointF movement = direction.normalized() * getMagneticSpeed() * speedMultiplier * deltaSec;
    setPosition(currentPos + movement);
}

void Coin::onCollideWithPlayer() {
    if (!isActive()) return; // 避免重复处理

    // 标记为已收集（父类方法）
    setActive(false);
    setVisible(false);

    // 播放收集音效（使用父类的m_soundEffect）
    if (m_soundEffect && m_soundEffect->status() == QSoundEffect::Ready) {
        m_soundEffect->play();
    }

    // 延迟删除（确保音效播放完成）
    QTimer::singleShot(500, this, &Coin::deleteLater);
}

void Coin::setGoldenEffect(bool enabled) {
    m_hasGoldenEffect = enabled;
    m_coinValue = enabled ? 5 : 1; // 金色金币价值为普通的5倍
    update(); // 触发重绘更新视觉效果
}

void Coin::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    if (!isActive()) return; // 非活跃状态不绘制

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿

    // 坐标系转换：以中心为原点进行旋转缩放
    painter.translate(rect().center());
    painter.rotate(m_currentRotation);
    painter.scale(m_currentScale, m_currentScale);
    painter.translate(-rect().center());

    // 绘制金币图像（使用父类的pixmap）
    if (!pixmap().isNull()) {
        painter.drawPixmap(rect(), pixmap());
    } else {
        // 图像加载失败时绘制默认金币
        painter.setBrush(m_hasGoldenEffect ? Qt::yellow : QColor(218, 165, 32));
        painter.drawEllipse(rect().adjusted(2, 2, -2, -2));
    }

    // 金色金币额外发光效果
    if (m_hasGoldenEffect) {
        QRadialGradient glow(rect().center(), rect().width()/2);
        glow.setColorAt(0, QColor(255, 215, 0, 100));  // 中心亮
        glow.setColorAt(1, QColor(255, 215, 0, 0));    // 边缘透明
        painter.setBrush(glow);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(rect());
    }
}

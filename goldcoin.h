#pragma once
#include "gameitem.h"
#include <QDateTime>
#include <QSoundEffect>

class Coin : public GameItem {
    Q_OBJECT

public:
    // 构造函数：与父类参数匹配，确保物品生成器能正确创建
    explicit Coin(QWidget* parent,
                  Track track,
                  QSize scaledSize = QSize(50, 50),
                  int speed = 120);

    // 重写磁力移动函数
    void moveTowardsTarget(qreal deltaSec) override;

    // 重写碰撞处理函数
    void onCollideWithPlayer() override;

    // 金币特有方法
    void setGoldenEffect(bool enabled);
    void setCoinValue(int value) { m_coinValue = value; }
    int getCoinValue() const { return m_coinValue; }

private:
    int m_coinValue = 1;          // 金币价值
    bool m_hasGoldenEffect = false; // 金色效果标记
    qint64 m_animationStartTime;  // 动画起始时间（用于计算旋转角度）
    qreal m_currentRotation = 0;  // 当前旋转角度
    qreal m_currentScale = 1.0;   // 当前缩放比例

    // 动画参数（常量，避免魔法数字）
    static constexpr int ROTATION_SPEED = 180;  // 每秒旋转角度
    static constexpr int SCALE_CYCLE = 1000;    // 缩放周期（毫秒）
    static constexpr qreal MAX_SCALE = 1.1;     // 最大缩放比例

    // 重绘事件：处理动画效果
    void paintEvent(QPaintEvent* event) override;
};

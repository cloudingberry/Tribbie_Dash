#pragma once
#include <QLabel>
#include <QSize>
#include <QGraphicsOpacityEffect>
#include <QSoundEffect>
#include <QPointF>

class GameItem : public QLabel {
    Q_OBJECT
public:
    enum Track {
        Top = 0,
        Middle = 1,
        Bottom = 2,
        BetweenTopMiddle = 3,
        BetweenMiddleBottom = 4
    };

    explicit GameItem(QWidget* parent,
                      const QString& imagePath,
                      Track track,
                      QSize scaledSize = QSize(80, 80),
                      int speed = 120);

    //绘制函数
    void setImage(const QString& imagePath);
    void setScaledSize(QSize size);

    //存在函数
    void setActive(bool active);
    bool isActive() const;

    //运动函数
    void updatePosition(qreal deltaSec);
    void stop();
    void pause();
    void resume();

    //渐隐并删除函数
    void fadeAndDelete(int duration = 300);

    //获取缩小的碰撞区域
    QRect getHitBox() const;

    //设置音效的函数
    void setSound(const QString& soundPath, qreal volume = 1.0);

    // 磁力相关函数
    bool isMagnetic() const { return m_isMagnetic; }
    void setMagnetic(bool magnetic) { m_isMagnetic = magnetic; }

    bool isBeingAttracted() const { return m_isBeingAttracted; }
    void setBeingAttracted(bool attracted) { m_isBeingAttracted = attracted; }

    void setTargetPosition(const QPointF& target) { m_targetPosition = target; }
    QPointF getTargetPosition() const { return m_targetPosition; }

    void setMagneticSpeed(float speed) { m_magneticSpeed = speed; }
    float getMagneticSpeed() const { return m_magneticSpeed; }

    // 磁力移动函数 - 虚函数，子类可重载
    virtual void moveTowardsTarget(qreal deltaSec);

    // 获取和设置位置（以QPointF形式）
    QPointF getPosition() const;
    void setPosition(const QPointF& pos);

public slots:
    //碰撞的槽函数，子类重写，处理碰撞反馈
    Q_INVOKABLE virtual void onCollideWithPlayer() {}

signals:
    void reachedTarget();

protected:
    int   m_speed;
    QSize m_scaledSize;
    Track m_track;
    bool  m_isActive = true;
    QGraphicsOpacityEffect* m_opacityEffect = nullptr;
    QSoundEffect* m_soundEffect = nullptr;

    // 磁力相关属性
    bool m_isMagnetic = false;
    bool m_isBeingAttracted = false;
    QPointF m_targetPosition;
    float m_magneticSpeed = 300.0f;  // 磁力移动速度(像素/秒)
    bool m_isPaused = false;         // 暂停状态

    // 辅助函数
    QPointF getTrackPosition() const;  // 获取轨道对应的Y坐标

private:
    static constexpr float ARRIVAL_THRESHOLD = 10.0f;  // 到达目标的阈值
};

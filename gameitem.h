#pragma once
#include <QLabel>
#include <QSize>

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
                      int speed = 120);          // 像素/秒

    void setImage(const QString& imagePath);
    void setScaledSize(QSize size);

    void setActive(bool active);
    bool isActive() const;

    // 每帧由 GameWindow 调用，推进位置
    void updatePosition(qreal deltaSec);
    void stop();

    void fadeAndDelete(int duration = 300);

    QRect getHitBox() const;

    void pause();

    void resume();



public slots:
    // 子类重写，处理碰撞反馈
    Q_INVOKABLE virtual void onCollideWithPlayer() {}

protected:
    int   m_speed;
    QSize m_scaledSize;
    Track m_track;
    bool  m_isActive = true;
};

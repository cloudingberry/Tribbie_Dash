#pragma once
#include <QLabel>
#include <QSize>
#include <QGraphicsOpacityEffect>
#include <QSoundEffect>

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


public slots:
    //碰撞的槽函数，子类重写，处理碰撞反馈
    Q_INVOKABLE virtual void onCollideWithPlayer() {}

protected:
    int   m_speed;
    QSize m_scaledSize;
    Track m_track;
    bool  m_isActive = true;

    QGraphicsOpacityEffect* m_opacityEffect = nullptr;
    QSoundEffect* m_soundEffect = nullptr;
};

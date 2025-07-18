#pragma once

#include <QLabel>
#include <QSoundEffect>
#include <QElapsedTimer>
#include <QTimer>

class Character : public QLabel {
    Q_OBJECT
public:
    enum Type {
        AN = 1,
        NING = 2,
        BAO = 3
    };

    explicit Character(QWidget* parent = nullptr);

    void init(Type type, const QList<int>& trackYPositions, int yOffset);
    // type 设定角色类型和图片
    // trackYPositions 是 GameWindow 提供的轨道 y 坐标
    // yOffset 角色垂直偏移（让角色底部对齐轨道线）

    void switchType(Type type); // 切换角色

    void processJump(qreal deltaSec);
    void startJump();

    Type currentType() const { return m_currentType; } // 返回当前角色类型
    bool isJumping() const { return m_isJumping; }     // 是否处于跳跃中

    bool isInSpearMode() const { return m_inSpearMode; }    // 是否持有长矛
    void activateSpearMode(int durationSec = 10);        // 启动spear模式
    bool isDashing() const {return m_isDashing; };       // 是否在冲刺
    void triggerDash();                                  // 发起冲刺

    void activateMagnetMode(int durationSec);            //启动磁力模式
    bool isMagnetActive() const { return m_magnetMode; } //判断是否处于磁力模式

    QRect hitBox() const;

signals:
    void jumped();

    void spearModeChanged(bool active, int durationSec);  // 用于通知GameWindow更新UI
    void dashStarted();
    void dashEnded();
    void magnetModeChanged(bool active, int duration);

private slots:
    void endSpearMode();
    void endDash();


private:
    void playJumpSound();

    Type m_currentType;
    QList<int> m_trackYPositions;
    int m_yOffset;

    bool m_isJumping = false;
    bool m_canDoubleJump = false;

    qreal m_verticalVelocity = 0.0; // 跳跃时竖直速度
    QElapsedTimer m_jumpTimer;      // 跳跃计时器

    const int CHARACTER_SIZE = 120;          // 角色图片宽高
    const qreal GRAVITY = 4.0;               // 每秒速度变化
    const qreal FIRST_JUMP_FORCE = -36.0;    // 第一跳初速度
    const qreal SECOND_JUMP_FORCE = -54.0;   // 二段跳速度更强

    bool m_inSpearMode = false;
    bool m_isDashing = false;
    QTimer* m_spearTimer;
    QTimer* m_dashTimer;

    bool m_magnetMode = false;
    QTimer m_magnetTimer;
};

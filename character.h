#pragma once

#include <QLabel>
#include <QSoundEffect>
#include <QElapsedTimer>

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
    void switchType(Type type);
    void processJump(qreal deltaSec);
    void startJump();

    Type currentType() const { return m_currentType; }
    bool isJumping() const { return m_isJumping; }
    QRect hitBox() const;

signals:
    void jumped();

private:
    void playJumpSound();

    Type m_currentType;
    QList<int> m_trackYPositions;
    int m_yOffset;
    bool m_isJumping = false;
    bool m_canDoubleJump = false;
    qreal m_verticalVelocity = 0.0;
    QElapsedTimer m_jumpTimer;

    const int CHARACTER_SIZE = 120;
    const qreal GRAVITY = 4.0;
    const qreal FIRST_JUMP_FORCE = -36.0;
    const qreal SECOND_JUMP_FORCE = -54.0;
};

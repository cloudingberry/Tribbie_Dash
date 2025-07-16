#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QLabel>
#include <QList>
#include "gameitem.h"
#include "itemmanager.h"
#include "collisionmanager.h"
#include "lionshield.h"
#include "resultwindow.h"
#include "spearitem.h"

class GameWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit GameWindow(int level, QWidget *parent = nullptr);
    ~GameWindow();

    void registerGameItem(GameItem* item);
    int trackY(GameItem::Track track) const;

    void freezeGame();

signals:
    void gameFinished();
    void gameFailed();
    void closed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void initCharacter();
    void switchCharacter(int type);
    void playJumpSound();
    void processJump();
    void addCoin();
    void addLetter();
    void loseLife();
    void togglePause();

    QLabel* m_character;
    QList<int> m_trackYPositions;

    ItemManager* m_itemManager;
    CollisionManager* m_collisionManager;

    QTimer* m_gameTimer;
    QElapsedTimer m_frameTimer;
    QTimer m_jumpTimer;

    int m_currentType;
    bool m_isJumping = false;
    bool m_canDoubleJump = false;
    qreal m_verticalVelocity = 0.0;

    bool m_isPaused = false;

    int m_coinCount = 0;
    int m_letterCount = 0;
    int m_lives = 3;

    QLabel* m_coinIcon;
    QLabel* m_coinText;
    QLabel* m_letterIcon;
    QLabel* m_letterText;
    QList<QLabel*> m_lifeIcons;

    const int CHARACTER_SIZE = 120;
    const int Y_OFFSET = 60;
    const qreal GRAVITY = 4.0;
    const qreal FIRST_JUMP_FORCE = -36.0;
    const qreal SECOND_JUMP_FORCE = -54.0;

    enum CharacterType {
        AN = 1,
        NING = 2,
        BAO = 3
    };

    // 地面滚动部分
    QList<QLabel*> m_groundTiles;
    const int GROUND_SPEED = 180;
    const int GROUND_TILE_HEIGHT = 80;  // 高度固定
    int m_groundTileWidth = 0;          // 宽度按比例计算，构造函数中初始化

    //长矛
    bool m_isSpearActive = false;    // 是否激活冲刺
    int m_spearCountdown = 0;        // 倒计时（帧数）

    QSoundEffect* m_spearSound;
    spearitem* m_spearItem;
    LionShield* m_shielditem;

    void activateSpear();            // 激活冲刺
    void updateSpearState();         // 每帧更新冲刺状态

    // 处理特殊碰撞（长矛vs盾牌）
    void handleCustomCollision(GameItem* a, GameItem* b);

    // 游戏状态管理
    void setGamePaused(bool paused);  // 统一暂停/继续逻辑
};

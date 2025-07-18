#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QLabel>
#include <QList>
#include "gameitem.h"
#include "itemmanager.h"
#include "collisionmanager.h"
#include "character.h"

class GameWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit GameWindow(int level, QWidget *parent = nullptr);
    ~GameWindow();

    void registerGameItem(GameItem* item);    // 注册物品到碰撞检测器中，并检查是否是 Receiver（终点）
    int trackY(GameItem::Track track) const;  // 获取某一轨道的 Y 坐标，用于放置物品或角色

    void freezeGame();      // 游戏冻结函数，暂停所有内容

signals:
    void gameFinished();
    void gameFailed();
    void closed();

protected:
    void paintEvent(QPaintEvent* event) override;   // 重写绘图事件（绘制背景）
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;  // 键盘事件（跳跃/切换角色/暂停等操作）

private:
    void initCharacter();
    void switchCharacter(int type);
    void processJump();

    void addCoin();
    void addLetter();
    void loseLife();

    void togglePause();

    Character* m_character;
    QList<int> m_trackYPositions;

    ItemManager* m_itemManager;
    CollisionManager* m_collisionManager;

    QTimer* m_gameTimer;
    QElapsedTimer m_frameTimer;

    bool m_isPaused = false;

    int m_coinCount = 0;
    int m_letterCount = 0;
    int m_lives = 3;

    QLabel* m_coinIcon;
    QLabel* m_coinText;
    QLabel* m_letterIcon;
    QLabel* m_letterText;
    QList<QLabel*> m_lifeIcons;

    const int Y_OFFSET = 60;

    // 地面滚动部分
    QList<QLabel*> m_groundTiles;
    const int GROUND_SPEED = 120;
    const int GROUND_TILE_HEIGHT = 80;  // 高度固定
    int m_groundTileWidth = 0;          // 宽度按比例计算，构造函数中初始化

    QLabel* m_spearIcon;
    QLabel* m_spearTimerLabel;
    QTimer* m_spearCountdownTimer;
    int m_spearRemainingTime = 0;
};

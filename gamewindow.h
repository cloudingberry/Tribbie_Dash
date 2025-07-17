#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QLabel>
#include <QList>
#include "gameitem.h"
#include "itemmanager.h"
#include "collisionmanager.h"
#include "resultwindow.h"
#include "character.h"

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
    const int GROUND_SPEED = 180;
    const int GROUND_TILE_HEIGHT = 80;  // 高度固定
    int m_groundTileWidth = 0;          // 宽度按比例计算，构造函数中初始化
};

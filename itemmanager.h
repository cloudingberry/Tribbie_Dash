#pragma once

#include "gameitem.h"
#include <QObject>
#include <QTimer>
#include <QList>

//前向声明,只用到了 GameWindow* 和 ItemPatternGenerator* 的 指针（而不是类的具体内容）
class GameWindow;
class ItemPatternGenerator;

class ItemManager : public QObject {
    Q_OBJECT

public:
    explicit ItemManager(QObject* parent, GameWindow* gameWindow, int level);

    void start();
    void stop();

    void pauseItems();
    void resumeItems();


private slots:
    void spawnNextPattern();   // 定时生成新套路

private:
    GameWindow* m_gameWindow;                // 指向主窗口
    QTimer* m_spawnTimer;                    // 定时器，控制生成节奏
    QList<GameItem*> m_items;                // 当前已生成的所有物品
    ItemPatternGenerator* m_patternGenerator;// 套路生成器

    int m_level = 1;
};

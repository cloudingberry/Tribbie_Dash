#pragma once

#include <QObject>
#include <QList>
#include <QDateTime>
#include "gameitem.h"

class GameWindow;

class ItemPatternGenerator : public QObject {
    Q_OBJECT
public:
    explicit ItemPatternGenerator(QObject* parent, GameWindow* gameWindow);

    QList<GameItem*> generateNextPattern(); // “外部接口”：被 ItemManager 调用，生成下一组物品

private:
    GameWindow* m_gameWindow;         // 所有物品要放入的游戏窗口
    int m_lastPatternIndex = -1;      // 上一次生成的套路编号，用于避免重复
    int m_letterCount = 0;            // 已经生成的信件数量（最多3）
    bool m_goalGenerated = false;     // 是否已生成终点 Receiver
    qint64 m_lastSpawnTime = 0;       // 上一次生成物品的时间
    qint64 m_minIntervalMs = 1500;    // 两次生成的最小间隔
    QVector<int> candidateTypes;      //存储当前可以选择生成的套路编号，并调整概率

    QList<GameItem*> generatePattern(int type);// 直接生成输入的编号的套路的函数

    QList<GameItem*> generateStraightCoins();
    QList<GameItem*> generateArcCoins();
    QList<GameItem*> generateShieldLine();
    QList<GameItem*> generateLetterWithTrap();
    QList<GameItem*> generateLetterGuard();
    QList<GameItem*> generateSpear();

    QList<GameItem*> generateGoal();
};

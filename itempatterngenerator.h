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
    QList<GameItem*> generateNextPattern();

private:
    GameWindow* m_gameWindow;
    int m_lastPatternIndex = -1;
    int m_letterCount = 0;
    bool m_goalGenerated = false;
    qint64 m_lastSpawnTime = 0;
    qint64 m_minIntervalMs = 1500;

    QList<GameItem*> generatePattern(int type);
    QList<GameItem*> generateStraightCoins();
    QList<GameItem*> generateArcCoins();
    QList<GameItem*> generateShieldLine();
    QList<GameItem*> generateLetterWithTrap();
    QList<GameItem*> generateLetterGuard();
    QList<GameItem*> generateGoal();
};

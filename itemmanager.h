#pragma once

#include "gameitem.h"
#include <QObject>
#include <QTimer>
#include <QList>

class GameWindow;
class ItemPatternGenerator;

class ItemManager : public QObject {
    Q_OBJECT

public:
    explicit ItemManager(QObject* parent, GameWindow* gameWindow);

    void start();
    void stop();

    void pauseItems();
    void resumeItems();


private slots:
    void spawnNextPattern();

private:
    GameWindow* m_gameWindow;
    QTimer* m_spawnTimer;
    QList<GameItem*> m_items;
    ItemPatternGenerator* m_patternGenerator;
};

// itemmanager.cpp
#include "itemmanager.h"
#include "gamewindow.h"
#include "itempatterngenerator.h"
#include "gameitem.h"

#include <algorithm>  // for std::max

ItemManager::ItemManager(QObject* parent, GameWindow* gameWindow)
    : QObject(parent), m_gameWindow(gameWindow)
{
    m_patternGenerator = new ItemPatternGenerator(this, gameWindow);

    m_spawnTimer = new QTimer(this);
    connect(m_spawnTimer, &QTimer::timeout, this, &ItemManager::spawnNextPattern);
}

void ItemManager::start() {
    m_spawnTimer->start(100);  // 改为高频调用，但内部判断是否生成
}

void ItemManager::stop() {
    m_spawnTimer->stop();
}

void ItemManager::spawnNextPattern() {
    const int minGap = 300;
    int maxRight = 0;

    for (auto* item : m_gameWindow->findChildren<GameItem*>()) {
        if (item && item->isActive()) {
            int rightEdge = item->x() + item->width();
            maxRight = std::max(maxRight, rightEdge);
        }
    }

    // 若最右边的物体还在距右边 <300px 以内，则不生成新套路
    if (maxRight > m_gameWindow->width() - minGap) {
        return;
    }

    auto items = m_patternGenerator->generateNextPattern();
    for (auto* item : items) {
        if (item) {
            item->setParent(m_gameWindow);
            item->show();
            m_gameWindow->registerGameItem(item);
            m_items.append(item);
        }
    }
}

void ItemManager::pauseItems() {
    for (GameItem* item : std::as_const(m_items)) {
        if (item) item->pause();
    }
}

void ItemManager::resumeItems() {
    for (GameItem* item : std::as_const(m_items)) {
        if (item) item->resume();
    }
}

// 新增：实现提高速度的方法

void ItemManager::increaseSpeed(int increment) {
    for (GameItem* item : std::as_const(m_items)) {
        if (item) {
            int newSpeed = item->getSpeed() + increment;
            item->setSpeed(newSpeed);
        }
    }
}

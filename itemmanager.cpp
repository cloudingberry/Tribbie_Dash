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
    const int minGap = 200;
    int maxRight = 0;  

    //找出最右边的物品的 x + width 值
    for (auto* item : m_gameWindow->findChildren<GameItem*>()) {
        if (item && item->isActive()) {
            int rightEdge = item->x() + item->width();
            maxRight = std::max(maxRight, rightEdge);
        }
    }

    // 若最右边的物体还在距右边 <200px 以内，则不生成新套路
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


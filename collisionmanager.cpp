#include "collisionmanager.h"
#include <algorithm>
#include <QMetaObject>

CollisionManager::CollisionManager(QObject* parent)
    : QObject(parent) {}

void CollisionManager::registerItem(GameItem* item) {
    if (item)
        m_items.append(item);
}

void CollisionManager::checkCollisions(const QRect& playerBox) {
    for (GameItem* item : m_items) {
        if (!item || !item->isActive()) continue;

        if (item->getHitBox().intersects(playerBox)) {
            item->setActive(false);

            // 先发信号，再异步调用物品自己的处理（避免立即 deleteLater 后访问 item）
            emit itemCollided(item);
            QMetaObject::invokeMethod(item, "onCollideWithPlayer", Qt::QueuedConnection);
        }
    }

    // 清除已失效或 nullptr 的物体
    m_items.erase(std::remove_if(m_items.begin(), m_items.end(),
                                 [](GameItem* item) { return !item || !item->isActive(); }),
                  m_items.end());
}

void CollisionManager::clearItems() {
    m_items.clear();
}
//好像没用上？

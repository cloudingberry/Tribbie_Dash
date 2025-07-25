#pragma once

#include <QObject>
#include <QList>
#include "gameitem.h"

class CollisionManager : public QObject {
    Q_OBJECT
public:
    explicit CollisionManager(QObject* parent = nullptr);

    // 注册一个新物品
    void registerItem(GameItem* item);

    // 清空已注册物品
    void clearItems();

    // 执行碰撞检测
    void checkCollisions(const QRect& playerRect);

signals:
    void itemCollided(GameItem* item); // 发出：某个物品被撞到信号

private:
    QList<GameItem*> m_items; // 当前所有需要检测的物品列表
};

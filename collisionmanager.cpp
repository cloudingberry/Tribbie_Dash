// collisionmanager.cpp
#include "collisionmanager.h"
#include <algorithm>
#include <QMetaObject>
#include <QDebug>
#include <cmath>
#include "gameitem.h" // 确保包含 GameItem 的完整定义

CollisionManager::CollisionManager(QObject* parent)
    : QObject(parent)
    , m_magneticActive(false)
    , m_magneticRange(150.0f)
    , m_playerPosition(0, 0)
    , m_soundEnabled(true)
    , m_coinCollectSound(nullptr)
    , m_magneticActivateSound(nullptr)
    , m_magneticTimer(nullptr)
    , m_updateTimer(nullptr)
{
    // 延迟初始化，确保对象完全构造后执行
    QTimer::singleShot(0, this, [this]() {
        initializeTimers();
        initializeSounds();
        qDebug() << "CollisionManager fully initialized";
    });
}

CollisionManager::~CollisionManager()
{
    // 停止所有定时器
    if (m_magneticTimer) {
        m_magneticTimer->stop();
        delete m_magneticTimer;
    }

    if (m_updateTimer) {
        m_updateTimer->stop();
        delete m_updateTimer;
    }

    // 清理物品列表
    clearItems();

    // 释放音效资源
    delete m_coinCollectSound;
    delete m_magneticActivateSound;

    qDebug() << "CollisionManager destroyed";
}

void CollisionManager::initializeTimers()
{
    // 初始化磁性效果定时器
    m_magneticTimer = new QTimer(this);
    m_magneticTimer->setSingleShot(true);
    connect(m_magneticTimer, &QTimer::timeout, this, &CollisionManager::onMagneticTimeout);

    // 初始化更新定时器 - 约60FPS
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &CollisionManager::updateMagneticEffect);
    m_updateTimer->start(16); // 16ms ≈ 60FPS

    qDebug() << "Timers initialized";
}

void CollisionManager::initializeSounds()
{
    // 释放旧的音效资源
    delete m_coinCollectSound;
    delete m_magneticActivateSound;

    // 初始化金币收集音效
    m_coinCollectSound = new QSoundEffect(this);
    m_coinCollectSound->setSource(QUrl::fromLocalFile(":/sounds/coin_collect.wav"));
    m_coinCollectSound->setVolume(0.5f);

    // 初始化磁性激活音效
    m_magneticActivateSound = new QSoundEffect(this);
    m_magneticActivateSound->setSource(QUrl::fromLocalFile(":/sounds/magnetic_activate.wav"));
    m_magneticActivateSound->setVolume(0.7f);

    // 尝试预加载
    m_coinCollectSound->play();
    m_coinCollectSound->stop();

    m_magneticActivateSound->play();
    m_magneticActivateSound->stop();

    // 检查加载状态
    bool coinReady = (m_coinCollectSound->status() == QSoundEffect::Ready);
    bool magneticReady = (m_magneticActivateSound->status() == QSoundEffect::Ready);

    if (!coinReady) {
        qDebug() << "Warning: coin_collect.wav not loaded. Status:" << m_coinCollectSound->status();
        qDebug() << "Check if the resource file exists and is properly configured.";
    }

    if (!magneticReady) {
        qDebug() << "Warning: magnetic_activate.wav not loaded. Status:" << m_magneticActivateSound->status();
        qDebug() << "Check if the resource file exists and is properly configured.";
    }

    // 如果至少有一个音效加载成功，则启用音效系统
    m_soundEnabled = coinReady || magneticReady;

    qDebug() << "Sound system initialized. Enabled:" << m_soundEnabled;
}

void CollisionManager::registerItem(GameItem* item) {
    if (!item) {
        qDebug() << "Error: Attempt to register null GameItem";
        return;
    }

    // 使用互斥锁确保线程安全
    QMutexLocker locker(&m_itemsMutex);

    // 避免重复注册
    if (m_items.contains(item)) {
        qDebug() << "Warning: GameItem already registered";
        return;
    }

    m_items.append(item);

    // 连接到达目标的信号
    connect(item, &GameItem::reachedTarget, this, [this, item]() {
        if (item && item->isActive()) {
            QRect playerRect(m_playerPosition.x() - 20, m_playerPosition.y() - 20, 40, 40);
            checkCollisions(playerRect);
        }
    }, Qt::QueuedConnection);

    // 监听物品销毁信号
    connect(item, &QObject::destroyed, this, [this, item]() {
        QMutexLocker locker(&m_itemsMutex);
        m_items.removeOne(item);
        qDebug() << "GameItem removed from manager";
    }, Qt::QueuedConnection);

    qDebug() << "GameItem registered successfully";
}

void CollisionManager::clearItems() {
    QMutexLocker locker(&m_itemsMutex);

    // 断开所有连接
    for (GameItem* item : m_items) {
        if (item) {
            disconnect(item, nullptr, this, nullptr);
        }
    }

    m_items.clear();
    qDebug() << "All GameItems cleared";
}

void CollisionManager::checkCollisions(const QRect& playerBox) {
    QMutexLocker locker(&m_itemsMutex);

    // 使用迭代器安全地遍历和删除
    for (auto it = m_items.begin(); it != m_items.end(); ) {
        GameItem* item = *it;

        if (!item) {
            it = m_items.erase(it);
            continue;
        }

        if (!item->isActive()) {
            // 安全删除非活跃物品
            it = m_items.erase(it);
            continue;
        }

        if (item->getHitBox().intersects(playerBox)) {
            item->setActive(false);

            // 播放碰撞音效
            playSound("coin_collect.wav");

            // 先发信号，再异步调用物品自己的处理
            emit itemCollided(item);

            // 使用 Qt::QueuedConnection 确保线程安全
            QMetaObject::invokeMethod(item, "onCollideWithPlayer", Qt::QueuedConnection);

            // 从列表中移除
            it = m_items.erase(it);

            qDebug() << "Item collision processed and removed";
        } else {
            ++it;
        }
    }
}

void CollisionManager::activateMagnetic(float duration) {
    if (duration <= 0) {
        qDebug() << "Error: Invalid magnetic duration:" << duration;
        return;
    }

    // 使用互斥锁确保线程安全
    QMutexLocker locker(&m_magneticMutex);

    if (m_magneticActive) {
        // 如果已经激活，重置定时器
        m_magneticTimer->stop();
    }

    m_magneticActive = true;
    m_magneticTimer->start(static_cast<int>(duration));

    // 播放激活音效
    playSound("magnetic_activate.wav");

    emit magneticActivated();

    qDebug() << "Magnetic effect activated for" << duration << "ms";
}

void CollisionManager::deactivateMagnetic() {
    // 使用互斥锁确保线程安全
    QMutexLocker locker(&m_magneticMutex);

    if (!m_magneticActive) return;

    m_magneticActive = false;
    m_magneticTimer->stop();

    // 停止所有物品的磁性移动
    for (GameItem* item : m_items) {
        if (item && item->isActive()) {
            item->setBeingAttracted(false);
        }
    }

    emit magneticDeactivated();

    qDebug() << "Magnetic effect deactivated";
}

void CollisionManager::updateMagneticEffect() {
    // 使用互斥锁确保线程安全
    QMutexLocker locker(&m_itemsMutex);
    QMutexLocker magneticLocker(&m_magneticMutex);

    if (!m_magneticActive) return;

    for (GameItem* item : m_items) {
        if (!item || !item->isActive()) continue;

        if (!item->isMagnetic()) continue;

        QPointF itemPos = item->getPosition();
        QPointF playerPos = m_playerPosition;

        // 计算距离
        float dx = itemPos.x() - playerPos.x();
        float dy = itemPos.y() - playerPos.y();
        float distance = sqrt(dx * dx + dy * dy);

        if (distance <= m_magneticRange) {
            // 在磁性范围内，开始吸引
            if (!item->isBeingAttracted()) {
                item->setBeingAttracted(true);
                qDebug() << "Item started being attracted";
            }

            // 设置目标位置并应用吸引力
            item->setTargetPosition(playerPos);
        } else {
            // 超出磁性范围，停止吸引
            if (item->isBeingAttracted()) {
                item->setBeingAttracted(false);
                qDebug() << "Item stopped being attracted";
            }
        }
    }
}

void CollisionManager::onMagneticTimeout() {
    deactivateMagnetic();
}

void CollisionManager::playSound(const QString& soundFile) {
    if (!m_soundEnabled) return;

    if (soundFile == "coin_collect.wav" && m_coinCollectSound
        && m_coinCollectSound->status() == QSoundEffect::Ready) {
        m_coinCollectSound->play();
    } else if (soundFile == "magnetic_activate.wav" && m_magneticActivateSound
               && m_magneticActivateSound->status() == QSoundEffect::Ready) {
        m_magneticActivateSound->play();
    }
}

// 设置玩家位置的公共方法
void CollisionManager::setPlayerPosition(const QPointF& position) {
    // 使用互斥锁确保线程安全
    QMutexLocker locker(&m_playerMutex);
    m_playerPosition = position;
}

// 获取玩家位置的公共方法
QPointF CollisionManager::getPlayerPosition() const {
    // 使用互斥锁确保线程安全
    QMutexLocker locker(&m_playerMutex);
    return m_playerPosition;
}

// 获取磁性状态的公共方法
bool CollisionManager::isMagneticActive() const {
    QMutexLocker locker(&m_magneticMutex);
    return m_magneticActive;
}

// 获取磁性范围的公共方法
float CollisionManager::getMagneticRange() const {
    QMutexLocker locker(&m_magneticMutex);
    return m_magneticRange;
}

// 设置磁性范围的公共方法
void CollisionManager::setMagneticRange(float range) {
    QMutexLocker locker(&m_magneticMutex);
    if (range > 0) {
        m_magneticRange = range;
    }
}

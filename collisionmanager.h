// collisionmanager.h
#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include <QObject>
#include <QTimer>
#include <QPointF>
#include <QRect>
#include <QSoundEffect>
#include <QMutex>
#include <QList>

// 前向声明 GameItem
class GameItem;

class CollisionManager : public QObject
{
    Q_OBJECT
public:
    explicit CollisionManager(QObject* parent = nullptr);
    ~CollisionManager();

    void registerItem(GameItem* item);
    void clearItems();
    void checkCollisions(const QRect& playerBox);
    void activateMagnetic(float duration);
    void deactivateMagnetic();
    void setPlayerPosition(const QPointF& position);
    QPointF getPlayerPosition() const;
    bool isMagneticActive() const;
    float getMagneticRange() const;
    void setMagneticRange(float range);

signals:
    void itemCollided(GameItem* item);
    void magneticActivated();
    void magneticDeactivated();

private slots:
    void onMagneticTimeout();
    void updateMagneticEffect();

private:
    void initializeTimers();
    void initializeSounds();
    void playSound(const QString& soundFile);

    bool m_magneticActive;
    float m_magneticRange;
    QPointF m_playerPosition;
    bool m_soundEnabled;

    QSoundEffect* m_coinCollectSound;
    QSoundEffect* m_magneticActivateSound;

    QTimer* m_magneticTimer;
    QTimer* m_updateTimer;

    QList<GameItem*> m_items;

    mutable QMutex m_itemsMutex;
    mutable QMutex m_magneticMutex;
    mutable QMutex m_playerMutex;
};

#endif // COLLISIONMANAGER_H

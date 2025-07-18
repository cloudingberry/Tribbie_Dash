#include "itempatterngenerator.h"
#include "gamewindow.h"
#include "goldcoin.h"
#include "lionshield.h"
#include "redcrystal.h"
#include "letter.h"
#include "receiver.h"
#include "spear.h"
#include "magnet.h"
#include "speedpig.h"
#include <QRandomGenerator>

ItemPatternGenerator::ItemPatternGenerator(QObject* parent, GameWindow* gameWindow, int level)
    : QObject(parent), m_gameWindow(gameWindow) ,m_level(level){}

QList<GameItem*> ItemPatternGenerator::generateNextPattern() {

    qint64 now = QDateTime::currentMSecsSinceEpoch();

    if (now - m_lastSpawnTime < m_minIntervalMs)    return {};
    if (m_goalGenerated)    return {};

    m_lastSpawnTime = now;

    static int spawnCounter = 0;
    spawnCounter++;

    if (m_letterCount < 3) {
        switch(m_level){
        case 1: // 新手入门4件
            candidateTypes << 0 << 0 << 0 << 1 << 1 << 1 << 2 << 2 << 2;
            break;
        case 2: // 上手长矛
            candidateTypes << 0 << 1 << 2 << 2 << 2 << 2 << 2 << 5 << 5 << 5 << 5 << 5;
            break;
        case 3: //  上手加速小猪
            candidateTypes << 0 << 0 << 1 << 1 << 2 << 2 << 2 << 7 << 7 << 7 << 7 << 7;
            break;
        case 4: // 上手吸铁石
            candidateTypes << 0 << 0 << 0 << 1 << 1 << 1 << 2 << 2 << 6 << 6 << 6 << 6 << 6;
            break;
        case 5:
            candidateTypes << 0 << 0 << 1 << 1 << 2 << 2 << 5 << 5 << 6 << 6 << 7 << 7;
            break;
        }
        if (spawnCounter > 10) {
            candidateTypes << 3 << 4;
        }
    } else {
        m_goalGenerated = true;
        return generateGoal();
    }


    int chosenType;
    do {
        chosenType = candidateTypes[QRandomGenerator::global()->bounded(candidateTypes.size())];
    } while (chosenType == m_lastPatternIndex && candidateTypes.size() > 1);

    m_lastPatternIndex = chosenType;
    return generatePattern(chosenType);

}

/*根据编号返回具体套路
    0 → 直线金币
    1 → 弧形金币
    2 → 直线盾牌
    3 → 信件陷阱
    4 → 信件守卫
    5 ->长矛
    6 ->吸铁石
    7 ->加速猪*/
QList<GameItem*> ItemPatternGenerator::generatePattern(int type) {
    switch (type) {
    case 0: return generateStraightCoins();
    case 1: return generateArcCoins();
    case 2: return generateShieldLine();
    case 3: return generateLetterWithTrap();
    case 4: return generateLetterGuard();
    case 5: return generateSpear();
    case 6: return generateMagnet();
    case 7: return generateSpeedPig();
    default: return {};
    }
}

QList<GameItem*> ItemPatternGenerator::generateStraightCoins() {
    QList<GameItem*> items;
    auto track = static_cast<GameItem::Track>(QRandomGenerator::global()->bounded(3));
    int baseX = m_gameWindow->width() + 50;

    for (int i = 0; i < 5; ++i) {
        auto* coin = new GoldCoin(m_gameWindow, track);
        coin->move(baseX + i * 90, m_gameWindow->trackY(track) - coin->height() / 2);
        items.append(coin);
    }
    return items;
}

QList<GameItem*> ItemPatternGenerator::generateArcCoins() {
    QList<GameItem*> items;

    struct CoinPoint {
        int dx;    // 相对 baseX 的偏移量
        int track; // 轨道编号（0~4）
    };

    // 定义弧形金币的布置
    QVector<QVector<CoinPoint>> arcPatterns = {
        { {0,2}, {19,4}, {43,1}, {75,3}, {150,0}, {225,3}, {257,1}, {281,4}, {300,2} }, // 弧线1
        { {0,2}, {48,4}, {104,1}, {160,4}, {208,2} }                    // 弧线2
    };

    const QVector<CoinPoint>& chosen = arcPatterns[QRandomGenerator::global()->bounded(arcPatterns.size())];

    int baseX = m_gameWindow->width() + 50;

    for (const CoinPoint& pt : chosen) {
        auto track = static_cast<GameItem::Track>(pt.track);
        auto* coin = new GoldCoin(m_gameWindow, track);
        coin->move(baseX + pt.dx, m_gameWindow->trackY(track) - coin->height() / 2);
        items.append(coin);
    }

    return items;
}



QList<GameItem*> ItemPatternGenerator::generateShieldLine() {
    QList<GameItem*> items;
    auto track = static_cast<GameItem::Track>(QRandomGenerator::global()->bounded(3));
    int baseX = m_gameWindow->width() + 50;

    for (int i = 0; i < 3; ++i) {
        auto* shield = new LionShield(m_gameWindow, track);
        shield->move(baseX + i * 90, m_gameWindow->trackY(track) - shield->height() / 2);
        items.append(shield);
    }
    return items;
}

QList<GameItem*> ItemPatternGenerator::generateLetterWithTrap() {
    QList<GameItem*> items;
    auto track = static_cast<GameItem::Track>(QRandomGenerator::global()->bounded(3));
    int baseX = m_gameWindow->width() + 50;

    auto* letter = new Letter(m_gameWindow, track);
    letter->move(baseX, m_gameWindow->trackY(track) - letter->height() / 2);
    items.append(letter);

    auto* trap = new RedCrystal(m_gameWindow, track);
    trap->move(baseX + 100, m_gameWindow->trackY(track) - trap->height() / 2);
    items.append(trap);

    ++m_letterCount;
    return items;
}


QList<GameItem*> ItemPatternGenerator::generateLetterGuard() {
    QList<GameItem*> items;
    GameItem::Track center = static_cast<GameItem::Track>(QRandomGenerator::global()->bounded(3));
    int baseX = m_gameWindow->width() + 50;

    auto* letter = new Letter(m_gameWindow, center);
    letter->move(baseX , m_gameWindow->trackY(center) - letter->height() / 2);
    items.append(letter);
    ++m_letterCount;

    if (center != GameItem::Top) {
        auto* shield = new LionShield(m_gameWindow, static_cast<GameItem::Track>(center - 1));
        shield->move(baseX , m_gameWindow->trackY(static_cast<GameItem::Track>(center - 1)) - shield->height() / 2);
        items.append(shield);
    }

    if (center != GameItem::Bottom) {
        auto* trap = new RedCrystal(m_gameWindow, static_cast<GameItem::Track>(center + 1));
        trap->move(baseX , m_gameWindow->trackY(static_cast<GameItem::Track>(center + 1)) - trap->height() / 2);
        items.append(trap);
    }

    return items;
}

QList<GameItem*> ItemPatternGenerator::generateGoal() {
    QList<GameItem*> items;
    auto track = static_cast<GameItem::Track>(QRandomGenerator::global()->bounded(3));
    auto* receiver = new Receiver(m_gameWindow, track);
    receiver->move(m_gameWindow->width() + 50, m_gameWindow->trackY(track) - receiver->height() / 2);
    items.append(receiver);
    return items;
}

QList<GameItem*> ItemPatternGenerator::generateSpear() {
    QList<GameItem*> items;
    auto track = static_cast<GameItem::Track>(QRandomGenerator::global()->bounded(3));
    auto* spear = new Spear(m_gameWindow, track);
    spear->move(m_gameWindow->width() + 50, m_gameWindow->trackY(track) - spear->height() / 2);
    items.append(spear);
    return items;
}

QList<GameItem*> ItemPatternGenerator::generateMagnet() {
    QList<GameItem*> items;
    auto track = static_cast<GameItem::Track>(QRandomGenerator::global()->bounded(3));
    auto* magnet = new Magnet(m_gameWindow, track);
    magnet->move(m_gameWindow->width() + 50, m_gameWindow->trackY(track) - magnet->height() / 2);
    items.append(magnet);
    return items;
}

QList<GameItem*> ItemPatternGenerator::generateSpeedPig() {
    QList<GameItem*> items;
    auto track = static_cast<GameItem::Track>(QRandomGenerator::global()->bounded(3));
    auto* speedpig = new SpeedPig(m_gameWindow, track);
    speedpig->move(m_gameWindow->width() + 50, m_gameWindow->trackY(track) - speedpig->height() / 2);
    items.append(speedpig);
    return items;
}

#include "itempatterngenerator.h"
#include "gamewindow.h"
#include "goldcoin.h"  // 假设实际类名为Coin，此处为头文件
#include "lionshield.h"
#include "redcrystal.h"
#include "letter.h"
#include "receiver.h"
#include <QRandomGenerator>

// 生成直线排列的金币（类名修复版）
QList<GameItem*> ItemPatternGenerator::generateStraightCoins() {
    QList<GameItem*> items;
    int trackInt = QRandomGenerator::global()->bounded(3);
    GameItem::Track track = static_cast<GameItem::Track>(trackInt);
    int baseX = m_gameWindow->width() + 50;  // 从窗口右侧外部开始生成

    for (int i = 0; i < 6; ++i) {
        // 关键修复：使用正确的类名（如果头文件中定义为Coin）
        auto* coin = new Coin(  // 此处使用Coin而非GoldCoin
            m_gameWindow,       // 父窗口
            track,              // 轨道
            QSize(50, 50),      // 大小
            120                 // 速度
            );

        // 计算Y坐标（轨道中心 - 金币半高）
        int trackY = m_gameWindow->trackY(track);
        int yPos = trackY - (coin->height() / 2);
        coin->move(baseX + (i * 90), yPos);  // X方向间隔90像素
        items.append(coin);
    }
    return items;
}

// 生成弧形排列的金币（类名修复版）
QList<GameItem*> ItemPatternGenerator::generateArcCoins() {
    QList<GameItem*> items;
    QVector<GameItem::Track> tracks = {
        GameItem::Bottom,
        GameItem::Middle,
        GameItem::Top
    };
    int baseX = m_gameWindow->width() + 50;
    int offset = QRandomGenerator::global()->bounded(2) == 0 ? 1 : -1;

    for (int i = 0; i < 3; ++i) {
        // 计算有效轨道索引
        int trackIndex = (i + 3 + offset) % 3;
        trackIndex = qMax(0, qMin(2, trackIndex));  // 确保在0-2范围内
        GameItem::Track track = tracks[trackIndex];

        // 关键修复：使用正确的类名Coin
        auto* coin = new Coin(
            m_gameWindow,
            track,
            QSize(50, 50),
            120
            );

        int trackY = m_gameWindow->trackY(track);
        int yPos = trackY - (coin->height() / 2);
        coin->move(baseX + (i * 100), yPos);  // X方向间隔100像素
        items.append(coin);
    }
    return items;
}

// 其他函数保持不变（完整实现）
ItemPatternGenerator::ItemPatternGenerator(QObject* parent, GameWindow* gameWindow)
    : QObject(parent), m_gameWindow(gameWindow), m_lastSpawnTime(0),
    m_goalGenerated(false), m_letterCount(0), m_lastPatternIndex(-1) {}

QList<GameItem*> ItemPatternGenerator::generateNextPattern() {
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (now - m_lastSpawnTime < 800 || m_goalGenerated) return {};

    m_lastSpawnTime = now;
    static int spawnCounter = 0;
    spawnCounter++;

    QVector<int> candidateTypes;
    if (m_letterCount < 3) {
        candidateTypes << 0 << 0 << 1 << 1 << 2 << 2;
        if (spawnCounter > 5) candidateTypes << 3 << 4;
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

QList<GameItem*> ItemPatternGenerator::generatePattern(int type) {
    switch (type) {
    case 0: return generateStraightCoins();
    case 1: return generateArcCoins();
    case 2: return generateShieldLine();
    case 3: return generateLetterWithTrap();
    case 4: return generateLetterGuard();
    default: return {};
    }
}

QList<GameItem*> ItemPatternGenerator::generateShieldLine() {
    QList<GameItem*> items;
    auto track = static_cast<GameItem::Track>(QRandomGenerator::global()->bounded(3));
    int baseX = m_gameWindow->width() + 50;

    for (int i = 0; i < 3; ++i) {
        auto* shield = new LionShield(m_gameWindow, track);
        int yPos = m_gameWindow->trackY(track) - (shield->height() / 2);
        shield->move(baseX + (i * 90), yPos);
        items.append(shield);
    }
    return items;
}

QList<GameItem*> ItemPatternGenerator::generateLetterWithTrap() {
    QList<GameItem*> items;
    auto track = static_cast<GameItem::Track>(QRandomGenerator::global()->bounded(3));
    int baseX = m_gameWindow->width() + 50;

    auto* letter = new Letter(m_gameWindow, track);
    int yPos = m_gameWindow->trackY(track) - (letter->height() / 2);
    letter->move(baseX, yPos);
    items.append(letter);

    auto* trap = new RedCrystal(m_gameWindow, track);
    trap->move(baseX + 100, yPos);
    items.append(trap);

    m_letterCount++;
    return items;
}

QList<GameItem*> ItemPatternGenerator::generateLetterGuard() {
    QList<GameItem*> items;
    auto centerTrack = static_cast<GameItem::Track>(QRandomGenerator::global()->bounded(3));
    int baseX = m_gameWindow->width() + 50;

    auto* letter = new Letter(m_gameWindow, centerTrack);
    int yPos = m_gameWindow->trackY(centerTrack) - (letter->height() / 2);
    letter->move(baseX + 90, yPos);
    items.append(letter);
    m_letterCount++;

    if (centerTrack != GameItem::Top) {
        auto upperTrack = static_cast<GameItem::Track>(static_cast<int>(centerTrack) - 1);
        auto* shield = new LionShield(m_gameWindow, upperTrack);
        int shieldY = m_gameWindow->trackY(upperTrack) - (shield->height() / 2);
        shield->move(baseX, shieldY);
        items.append(shield);
    }

    if (centerTrack != GameItem::Bottom) {
        auto lowerTrack = static_cast<GameItem::Track>(static_cast<int>(centerTrack) + 1);
        auto* trap = new RedCrystal(m_gameWindow, lowerTrack);
        int trapY = m_gameWindow->trackY(lowerTrack) - (trap->height() / 2);
        trap->move(baseX + 180, trapY);
        items.append(trap);
    }

    return items;
}

QList<GameItem*> ItemPatternGenerator::generateGoal() {
    QList<GameItem*> items;
    auto track = static_cast<GameItem::Track>(QRandomGenerator::global()->bounded(3));
    auto* receiver = new Receiver(m_gameWindow, track);
    int yPos = m_gameWindow->trackY(track) - (receiver->height() / 2);
    receiver->move(m_gameWindow->width() + 50, yPos);
    items.append(receiver);
    return items;
}

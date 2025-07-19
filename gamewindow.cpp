#include "gamewindow.h"
#include "receiver.h"
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QSoundEffect>
#include <QKeyEvent>
#include "goldcoin.h"
#include "letter.h"
#include "redcrystal.h"
#include "lionshield.h"
#include "resultwindow.h"
#include "spear.h"
#include "magnet.h"
#include <speedpig.h>
#include <QTimer>

GameWindow::GameWindow(int level, QWidget *parent)
    : QMainWindow(parent), m_level(level)
{
    setFixedSize(1280, 720);
    setFocusPolicy(Qt::StrongFocus);

    int baseY = height() * 0.25;
    int spacing = height() * 0.18;
    m_trackYPositions = { baseY, baseY + spacing, baseY + 2 * spacing }; // 三条轨道的中心 Y 坐标

    initCharacter();

    m_itemManager = new ItemManager(this, this, m_level);
    m_collisionManager = new CollisionManager(this);

    // 信号连接处理各种物品
    connect(m_collisionManager, &CollisionManager::itemCollided, this, [=](GameItem* item) {
        if (qobject_cast<GoldCoin*>(item)) {
            addCoin();
        } else if (qobject_cast<Letter*>(item)) {
            addLetter();
        } else if (qobject_cast<RedCrystal*>(item)) {
            loseLife();
        } else if (qobject_cast<LionShield*>(item)) {
            if (!m_character->isDashing()) {
                emit gameFailed();
            }
        }else if (qobject_cast<Spear*>(item)) {
            m_character->activateSpearMode(20);
        }else if (qobject_cast<Magnet*>(item)) {
            m_character->activateMagnetMode(15);
        }else if (qobject_cast<SpeedPig*>(item)) {
            m_character->activateSpeedUpMode(8);
        }
    });

    m_itemManager->start();
    m_frameTimer.start();

    m_gameTimer = new QTimer(this);

    //游戏主循环逻辑
    //每帧执行：跳跃处理,地面滚动,所有物品移动,碰撞检测
    connect(m_gameTimer, &QTimer::timeout, this, [=]() {
        qreal deltaSec = m_frameTimer.restart() / 1000.0;
        processJump();

        // 冲刺状态下速度加倍
        qreal speedMultiplier = 1.0;

        if (m_character->isDashing())
            speedMultiplier *= 4.5;

        if (m_character->isSpeedUpActive())
            speedMultiplier *= 2.0;

        int currentGroundSpeed = static_cast<int>(GROUND_SPEED * speedMultiplier);

        // 地面移动
        for (QLabel* ground : m_groundTiles) {
            int newX = ground->x() - static_cast<int>(currentGroundSpeed * deltaSec);
            if (newX + m_groundTileWidth < 0) {
                int rightmostX = 0;
                for (QLabel* other : m_groundTiles)
                    rightmostX = qMax(rightmostX, other->x());
                newX = rightmostX + m_groundTileWidth;
            }
            ground->move(newX, ground->y());
        }

        // 物品移动
        for (auto* item : findChildren<GameItem*>()) {
            item->updatePosition(deltaSec * speedMultiplier);

            // 检查收信人是否离开屏幕
            if (auto* receiver = qobject_cast<Receiver*>(item)) {
                if (receiver->x() + receiver->width() < 0) {
                    qDebug() << "Receiver missed! Game failed.";
                    emit gameFailed();
                    return;  // 防止多次触发
                }
            }
        }

        m_collisionManager->checkCollisions(m_character->hitBox());
    });

    m_gameTimer->start(16);

    // ========== UI积分区域 ==========
    m_coinIcon = new QLabel(this);
    m_coinIcon->setPixmap(QPixmap(":/images/coin.png").scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_coinIcon->setGeometry(width() / 2 + 200, 20, 40, 40);

    m_coinText = new QLabel("× 0", this);
    m_coinText->setStyleSheet("color: white; font: bold 20px;");
    m_coinText->setGeometry(width() / 2 + 250, 20, 60, 40);
    m_coinText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_letterIcon = new QLabel(this);
    m_letterIcon->setPixmap(QPixmap(":/images/letter.png").scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_letterIcon->setGeometry(width() / 2 + 430, 20, 40, 40);

    m_letterText = new QLabel("× 0", this);
    m_letterText->setStyleSheet("color: white; font: bold 20px;");
    m_letterText->setGeometry(width() / 2 + 480, 20, 60, 40);
    m_letterText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // ========== 生命显示 ==========
    int heartXStart = width() / 2 - 300;
    for (int i = 0; i < 3; ++i) {
        QLabel* heart = new QLabel(this);
        heart->setPixmap(QPixmap(":/images/heart_red.png").scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        heart->setGeometry(heartXStart + i * 50, 20, 40, 40);
        m_lifeIcons.append(heart);
    }

    // ========== 初始化地面 ==========
    // 地面视觉高度调低一点
    int groundTop = m_trackYPositions[2] + m_character->height() / 2 - Y_OFFSET + 35;

    // 调整地面尺寸
    const int GROUND_TILE_HEIGHT = 100;
    QPixmap groundPixmap(":/images/ground.png");
    qreal aspectRatio = static_cast<qreal>(groundPixmap.width()) / groundPixmap.height();
    m_groundTileWidth = static_cast<int>(GROUND_TILE_HEIGHT * aspectRatio);

    // 计算拼接数量
    int numTiles = width() / m_groundTileWidth + 2;

    // 创建地面图片
    for (int i = 0; i < numTiles; ++i) {
        QLabel* ground = new QLabel(this);
        ground->setPixmap(groundPixmap.scaled(m_groundTileWidth, GROUND_TILE_HEIGHT, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        ground->setGeometry(i * m_groundTileWidth, groundTop, m_groundTileWidth, GROUND_TILE_HEIGHT);
        m_groundTiles.append(ground);
    }

    // 连接游戏结束信号
    connect(this, &GameWindow::gameFailed, this, [=]() {
        freezeGame();

        QSoundEffect* failSound = new QSoundEffect(this);
        failSound->setSource(QUrl("qrc:/sound/fail.wav"));
        failSound->setVolume(0.8);
        failSound->play();

        auto* failWindow = new ResultWindow(false, m_coinCount, m_letterCount, m_lives, this);
        failWindow->move((width() - failWindow->width()) / 2, (height() - failWindow->height()) / 2);
        failWindow->show();

        connect(failWindow, &ResultWindow::retryRequested, this, [=]() {
            close();
            QTimer::singleShot(100, [=]() {
                GameWindow* newGame = new GameWindow(m_level);
                newGame->show();
            });
        });

        connect(failWindow, &ResultWindow::backToMenuRequested, this, [=]() {
            close();
        });
    });

    connect(this, &GameWindow::gameFinished, this, [=]() {
        freezeGame();
        auto* winWindow = new ResultWindow(true, m_coinCount, m_letterCount, m_lives, this);
        winWindow->move((width() - winWindow->width()) / 2, (height() - winWindow->height()) / 2);
        winWindow->show();

        QSoundEffect* failSound = new QSoundEffect(this);
        failSound->setSource(QUrl("qrc:/sound/fail.wav"));
        failSound->setVolume(0.8);
        failSound->play();

        connect(winWindow, &ResultWindow::retryRequested, this, [=]() {
            close();
            QTimer::singleShot(100, [=]() {
                GameWindow* newGame = new GameWindow(m_level);
                newGame->show();
            });
        });

        connect(winWindow, &ResultWindow::backToMenuRequested, this, [=]() {
            close();
        });
    });


    // Spear UI 初始化
    m_spearIcon = new QLabel(this);
    m_spearIcon->setPixmap(QPixmap(":/images/spear.png").scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_spearIcon->setGeometry(20, height() - 180, 200, 200);
    m_spearIcon->hide();

    m_spearTimerLabel = new QLabel(this);
    m_spearTimerLabel->setStyleSheet("color: white; font: bold 20px;");
    m_spearTimerLabel->setGeometry(220, height() - 70, 40, 40);
    m_spearTimerLabel->hide();

    m_spearCountdownTimer = new QTimer(this);
    connect(m_spearCountdownTimer, &QTimer::timeout, this, [=]() {
        m_spearRemainingTime--;
        if (m_spearRemainingTime <= 0) {
            m_spearCountdownTimer->stop();
        }
        m_spearTimerLabel->setText(QString("%1s").arg(m_spearRemainingTime));
    });

    connect(m_character, &Character::spearModeChanged, this, [=](bool active, int duration) {
        m_spearIcon->setVisible(active);
        m_spearTimerLabel->setVisible(active);
        if (active) {
            m_spearRemainingTime = duration;
            m_spearTimerLabel->setText(QString("%1s").arg(m_spearRemainingTime));
            m_spearCountdownTimer->start(1000);
        } else {
            m_spearCountdownTimer->stop();
        }
    });

    // Magnet UI 初始化
    m_magnetIcon = new QLabel(this);
    m_magnetIcon->setPixmap(QPixmap(":/images/magnet.png").scaled(90, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_magnetIcon->setGeometry(270, height() - 110, 90, 90);
    m_magnetIcon->hide();

    m_magnetTimerLabel = new QLabel(this);
    m_magnetTimerLabel->setStyleSheet("color: white; font: bold 20px;");
    m_magnetTimerLabel->setGeometry(380, height() - 70, 40, 40);
    m_magnetTimerLabel->hide();

    m_magnetCountdownTimer = new QTimer(this);
    connect(m_magnetCountdownTimer, &QTimer::timeout, this, [=]() {
        m_magnetRemainingTime--;
        if (m_magnetRemainingTime <= 0) {
            m_magnetCountdownTimer->stop();
        }
        m_magnetTimerLabel->setText(QString("%1s").arg(m_magnetRemainingTime));
    });

    connect(m_character, &Character::magnetModeChanged, this, [=](bool active, int duration) {
        m_magnetIcon->setVisible(active);
        m_magnetTimerLabel->setVisible(active);
        if (active) {
            m_magnetRemainingTime = duration;
            m_magnetTimerLabel->setText(QString("%1s").arg(m_magnetRemainingTime));
            m_magnetCountdownTimer->start(1000);
        } else {
            m_magnetCountdownTimer->stop();
        }
    });

    // 加速小猪UI初始化
    m_speedUpIcon = new QLabel(this);
    m_speedUpIcon->setPixmap(QPixmap(":/images/speedpig.png").scaled(90, 90));
    m_speedUpIcon->setGeometry(450, height() - 110, 90, 90);  // 放在长矛右侧
    m_speedUpIcon->hide();

    m_speedUpTimerLabel = new QLabel(this);
    m_speedUpTimerLabel->setStyleSheet("color: white; font-size: 20px;");
    m_speedUpTimerLabel->setGeometry(550, height() - 70, 40, 40);  // 紧随其右
    m_speedUpTimerLabel->hide();

    m_speedUpCountdownTimer = new QTimer(this);
    connect(m_speedUpCountdownTimer, &QTimer::timeout, this, [=]() {
        m_speedUpRemainingTime--;
        if (m_speedUpRemainingTime <= 0) {
            m_speedUpCountdownTimer->stop();
        }
        m_speedUpTimerLabel->setText(QString("%1s").arg(m_speedUpRemainingTime));
    });
    connect(m_character, &Character::speedUpModeChanged, this, [=](bool active, int duration) {
        m_speedUpIcon->setVisible(active);
        m_speedUpTimerLabel->setVisible(active);
        if (active) {
            m_speedUpRemainingTime = duration;
            m_speedUpTimerLabel->setText(QString("%1s").arg(m_speedUpRemainingTime));
            m_speedUpCountdownTimer->start(1000);  // 每秒刷新
        } else {
            m_speedUpCountdownTimer->stop();
        }
    });

    // 构造函数 GameWindow::GameWindow(int level, QWidget *parent) 的尾部添加：
    m_pauseOverlay = new QLabel(this);
    QString path = QString(":/images/pause%1.png").arg(m_level);
    m_pauseOverlay->setPixmap(QPixmap(path).scaled(1000, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_pauseOverlay->setGeometry((width() - 800)/2 -100 , (height() - 400) / 2, 1000, 500);  // 居中
    m_pauseOverlay->setStyleSheet("background: transparent;");
    m_pauseOverlay->hide();


    togglePause(); // 默认暂停

}

GameWindow::~GameWindow() {
    m_gameTimer->stop();
    delete m_gameTimer;
}

// 将生成的游戏物品注册到碰撞检测系统中，并连接 Receiver（终点线）特殊信号
void GameWindow::registerGameItem(GameItem* item) {
    m_collisionManager->registerItem(item);
    if (auto* receiver = qobject_cast<Receiver*>(item)) {
        connect(receiver, &Receiver::goalReached, this, [=]() {
            qDebug() << "到达终点！游戏结束";
            emit gameFinished();
        });
    }
}

// 根据传入的轨道枚举类型返回对应的 Y 坐标，用于定位物体或角色
int GameWindow::trackY(GameItem::Track track) const {
    switch (track) {
    case GameItem::Top: return m_trackYPositions[0];
    case GameItem::Middle: return m_trackYPositions[1];
    case GameItem::Bottom: return m_trackYPositions[2];
    case GameItem::BetweenTopMiddle: return (m_trackYPositions[0] + m_trackYPositions[1]) / 2;
    case GameItem::BetweenMiddleBottom: return (m_trackYPositions[1] + m_trackYPositions[2]) / 2;
    default: return m_trackYPositions[1];
    }
}

// 初始化角色对象 m_character，设置默认类型为 BAO，并设置轨道坐标与偏移
void GameWindow::initCharacter() {
    m_character = new Character(this);
    m_character->init(Character::BAO, m_trackYPositions, Y_OFFSET);
}

void GameWindow::switchCharacter(int type) {
    m_character->switchType(static_cast<Character::Type>(type));
}

void GameWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    QString path = QString(":/images/wfls%1.png").arg(m_level);
    painter.drawPixmap(rect(), QPixmap(path));
}


void GameWindow::closeEvent(QCloseEvent* event) {
    emit closed();
    QMainWindow::closeEvent(event);
}

void GameWindow::addCoin() {
    m_coinCount++;
    m_coinText->setText(QString("× %1").arg(m_coinCount));
}

void GameWindow::addLetter() {
    m_letterCount++;
    m_letterText->setText(QString("× %1").arg(m_letterCount));
}

void GameWindow::loseLife() {
    if (m_lives <= 0) return;

    m_lives--;
    if (m_lives < m_lifeIcons.size()) {
        m_lifeIcons[m_lives]->setPixmap(QPixmap(":/images/heart_gray.png").scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    if (m_lives == 0) {
        emit gameFailed();
    }
}

void GameWindow::togglePause() {
    m_isPaused = !m_isPaused;

    if (m_isPaused) {
        m_gameTimer->stop();
        m_itemManager->pauseItems();
        m_pauseOverlay->show(); // 显示暂停提示图
    } else {
        m_frameTimer.restart(); // 避免物品突进
        m_gameTimer->start(16);
        m_itemManager->resumeItems();
        m_pauseOverlay->hide(); // 隐藏暂停图
    }
}

void GameWindow::freezeGame() {
    if (!m_isPaused) {
        m_gameTimer->stop();
        m_itemManager->pauseItems();
        m_isPaused = true;
    }
}

void GameWindow::processJump() {
    qreal deltaSec = m_frameTimer.restart() / 1000.0;
    m_character->processJump(deltaSec);
}

void GameWindow::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_Space:
        m_character->startJump();
        break;
    case Qt::Key_1:
        switchCharacter(Character::BAO);
        break;
    case Qt::Key_2:
        switchCharacter(Character::NING);
        break;
    case Qt::Key_3:
        switchCharacter(Character::AN);
        break;
    case Qt::Key_S:
        togglePause();
        break;
    case Qt::Key_D:
        m_character->triggerDash();
        break;

    case Qt::Key_Escape:
        close();
        break;
    }
}

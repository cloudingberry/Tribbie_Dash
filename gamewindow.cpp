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
#include "levelselectwindow.h"
#include "resultwindow.h"
#include <QTimer>
#include "spearitem.h"

GameWindow::GameWindow(int level, QWidget *parent)
    : QMainWindow(parent)
{
    Q_UNUSED(level);
    setFixedSize(1280, 720);
    setFocusPolicy(Qt::StrongFocus);

    int baseY = height() * 0.25;
    int spacing = height() * 0.18;
    m_trackYPositions = { baseY, baseY + spacing, baseY + 2 * spacing };

    m_character = new QLabel(this);
    initCharacter();

    m_itemManager = new ItemManager(this, this);
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
            emit gameFailed();
        }
    });



    m_spearItem = new spearitem(this, GameItem::Track::Middle);
    m_shielditem = new LionShield(this, GameItem::Track::Middle);

    // 注册长矛和盾牌到碰撞系统
    m_collisionManager->registerItem(m_spearItem);

    // 连接自定义碰撞信号
    connect(m_collisionManager, &CollisionManager::customCollision, this, &GameWindow::handleCustomCollision);
}

    itemManager->start();
    m_frameTimer.start();

    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, [=]() {
        qreal deltaSec = m_frameTimer.restart() / 1000.0;
        processJump();

        for (QLabel* ground : m_groundTiles) {
            int newX = ground->x() - static_cast<int>(GROUND_SPEED * deltaSec);
            if (newX + m_groundTileWidth < 0) {
                int rightmostX = 0;
                for (QLabel* other : m_groundTiles)
                    rightmostX = qMax(rightmostX, other->x());
                newX = rightmostX + m_groundTileWidth;
            }
            ground->move(newX, ground->y());
        }


        for (auto* item : findChildren<GameItem*>()) {
            item->updatePosition(deltaSec);
        }

        m_collisionManager->checkCollisions(m_character->geometry());
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
    int groundTop = m_trackYPositions[2] + CHARACTER_SIZE / 2 - Y_OFFSET + 35;

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
        togglePause();
        auto* failWindow = new ResultWindow(false, m_coinCount, m_letterCount, m_lives, this);
        failWindow->move((width() - failWindow->width()) / 2, (height() - failWindow->height()) / 2);
        failWindow->show();

        connect(failWindow, &ResultWindow::retryRequested, this, [=]() {
            // 重启逻辑，待实现
        });
        connect(failWindow, &ResultWindow::backToMenuRequested, this, [=]() {
            // 返回菜单逻辑，待实现
        });
    });

    connect(this, &GameWindow::gameFinished, this, [=]() {
        togglePause();
        auto* winWindow = new ResultWindow(true, m_coinCount, m_letterCount, m_lives, this);
        winWindow->move((width() - winWindow->width()) / 2, (height() - winWindow->height()) / 2);
        winWindow->show();

        connect(winWindow, &ResultWindow::retryRequested, this, [=]() {
            // 重启逻辑，待实现
        });
        connect(winWindow, &ResultWindow::backToMenuRequested, this, [=]() {
            // 返回菜单逻辑，待实现
        });
    });


    connect(this, &GameWindow::gameFailed, this, [=]() {
        freezeGame();

        // ✅ 播放失败音效
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
                GameWindow* newGame = new GameWindow(1);
                newGame->show();
            });
        });

        connect(failWindow, &ResultWindow::backToMenuRequested, this, [=]() {
            close();  // 不再新建 LevelSelectWindow
        });
    });


    connect(this, &GameWindow::gameFinished, this, [=]() {
        freezeGame();
        auto* winWindow = new ResultWindow(true, m_coinCount, m_letterCount, m_lives, this);
        winWindow->move((width() - winWindow->width()) / 2, (height() - winWindow->height()) / 2);
        winWindow->show();

        connect(winWindow, &ResultWindow::retryRequested, this, [=]() {
            close();
            QTimer::singleShot(100, [=]() {
                GameWindow* newGame = new GameWindow(1);  // 替换为当前关卡
                newGame->show();
            });
        });

        connect(winWindow, &ResultWindow::backToMenuRequested, this, [=]() {
            close();  // ✅ 就是这里
        });
    });



}

GameWindow::~GameWindow() {
    m_gameTimer->stop();
    delete m_gameTimer;
}

void GameWindow::registerGameItem(GameItem* item) {
    m_collisionManager->registerItem(item);
    if (auto* receiver = qobject_cast<Receiver*>(item)) {
        connect(receiver, &Receiver::goalReached, this, [=]() {
            qDebug() << "到达终点！游戏结束";
            emit gameFinished();
        });
    }
}

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

void GameWindow::initCharacter() {
    QPixmap pixmap(":/images/bao.png");
    if (!pixmap.isNull()) {
        pixmap = pixmap.scaled(CHARACTER_SIZE, CHARACTER_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_character->setPixmap(pixmap);
    }

    m_character->setGeometry(
        width() / 2 - CHARACTER_SIZE / 2,
        m_trackYPositions[2] - Y_OFFSET,
        CHARACTER_SIZE, CHARACTER_SIZE
        );
    m_currentType = BAO;
}

void GameWindow::switchCharacter(int type) {
    if (type < AN || type > BAO || type == m_currentType) return;

    m_isJumping = false;
    m_verticalVelocity = 0;

    int trackIndex = type - 1;
    m_character->move(
        width() / 2 - CHARACTER_SIZE / 2,
        m_trackYPositions[trackIndex] - Y_OFFSET
        );

    QString imgPath;
    if (type == AN) imgPath = ":/images/an.png";
    else if (type == NING) imgPath = ":/images/ning.png";
    else imgPath = ":/images/bao.png";

    QPixmap pixmap(imgPath);
    if (!pixmap.isNull()) {
        pixmap = pixmap.scaled(CHARACTER_SIZE, CHARACTER_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_character->setPixmap(pixmap);
    }

    m_currentType = type;
}

void GameWindow::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_Space:
        if (m_currentType == AN) return;

        if (!m_isJumping) {
            m_verticalVelocity = FIRST_JUMP_FORCE;
            m_isJumping = true;
            m_canDoubleJump = true;
            playJumpSound();
        } else if (m_canDoubleJump && m_currentType == BAO) {
            m_verticalVelocity = SECOND_JUMP_FORCE;
            m_canDoubleJump = false;
            playJumpSound();
        }

        m_jumpTimer.start();
        break;

    case Qt::Key_1: switchCharacter(BAO); break;
    case Qt::Key_2: switchCharacter(NING); break;
    case Qt::Key_3: switchCharacter(AN); break;
    case Qt::Key_S: togglePause(); break;
    case Qt::Key_Escape: close(); break;
    }
}

void GameWindow::playJumpSound() {
    QSoundEffect* sound = new QSoundEffect(this);
    sound->setSource(QUrl("qrc:/sound/jump.wav"));
    sound->setVolume(0.8);
    sound->play();
}

void GameWindow::processJump() {
    if (m_currentType == AN) return;

    if (m_isJumping) {
        m_verticalVelocity += GRAVITY;
        int newY = m_character->y() + static_cast<int>(m_verticalVelocity);

        int targetY = m_trackYPositions[m_currentType - 1] - Y_OFFSET;

        if (newY <= m_trackYPositions[0] - Y_OFFSET) {
            newY = m_trackYPositions[0] - Y_OFFSET;
            m_verticalVelocity = 0;
        } else if (newY >= targetY) {
            newY = targetY;
            m_verticalVelocity = 0;
            m_isJumping = false;
            m_canDoubleJump = true;
        }

        m_character->move(m_character->x(), newY);
    }
}

void GameWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.drawPixmap(rect(), QPixmap(":/images/aohema1.png"));
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
    } else {
        m_frameTimer.restart(); // 避免物品突进
        m_gameTimer->start(16);
        m_itemManager->resumeItems();
    }
}

void GameWindow::freezeGame() {
    if (!m_isPaused) {
        m_gameTimer->stop();
        m_itemManager->pauseItems();
        m_isPaused = true;
    }
}


// 处理特殊碰撞类型（长矛vs盾牌）
void GameWindow::handleCustomCollision(GameItem* a, GameItem* b) {
    if ((a == m_spearItem && b == m_shielditem) ||
        (b == m_spearItem && a == m_shielditem )) {
        activateSpear();
    }
}

// 保持原有冲刺激活逻辑不变
void GameWindow::activateSpear() {
    m_isSpearActive = true;
    m_spearCountdown = 180;  // 3秒倒计时（60FPS）
    m_spearSound->play();
}


#include "resultwindow.h"
#include <QPixmap>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>

ResultWindow::ResultWindow(bool success, int coins, int letters, int lives, QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(600, 400);  // 界面悬浮大小
    setAttribute(Qt::WA_StyledBackground);
    setupUI(success, coins, letters, lives);
}

void ResultWindow::setupUI(bool success, int coins, int letters, int lives) {
    // 设置背景图
    m_background = new QLabel(this);
    QString bgPath = success ? ":/images/succeed.png" : ":/images/fail.png";
    m_background->setPixmap(QPixmap(bgPath).scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    m_background->setGeometry(0, 0, width(), height());

    // 半透明遮罩效果
    QLabel* mask = new QLabel(this);
    mask->setStyleSheet("background-color: rgba(0, 0, 0, 120);");
    mask->setGeometry(0, 0, width(), height());

    // 图标与信息展示（放在右侧）
    QVBoxLayout* infoLayout = new QVBoxLayout;

    auto* coin = new QLabel;
    coin->setPixmap(QPixmap(":/images/coin.png").scaled(40, 40));
    auto* coinText = new QLabel(QString("× %1").arg(coins));
    coinText->setStyleSheet("color: white; font: bold 20px;");
    auto* coinRow = new QHBoxLayout;
    coinRow->addWidget(coin);
    coinRow->addWidget(coinText);
    infoLayout->addLayout(coinRow);

    auto* letter = new QLabel;
    letter->setPixmap(QPixmap(":/images/letter.png").scaled(40, 40));
    auto* letterText = new QLabel(QString("× %1").arg(letters));
    letterText->setStyleSheet("color: white; font: bold 20px;");
    auto* letterRow = new QHBoxLayout;
    letterRow->addWidget(letter);
    letterRow->addWidget(letterText);
    infoLayout->addLayout(letterRow);

    auto* lifeRow = new QHBoxLayout;
    for (int i = 0; i < 3; ++i) {
        QLabel* heart = new QLabel;
        QString heartImg = (i < lives) ? ":/images/heart_red.png" : ":/images/heart_gray.png";
        heart->setPixmap(QPixmap(heartImg).scaled(30, 30));
        lifeRow->addWidget(heart);
    }
    infoLayout->addLayout(lifeRow);

    QWidget* infoWidget = new QWidget(this);
    infoWidget->setLayout(infoLayout);
    infoWidget->setGeometry(350, 100, 220, 150);

    // 重来 与 返回按钮
    m_retryButton = new QPushButton(this);
    m_retryButton->setIcon(QIcon(":/images/again.png"));
    m_retryButton->setIconSize(QSize(60, 60));
    m_retryButton->setFlat(true);
    m_retryButton->setGeometry(150, 300, 60, 60);
    connect(m_retryButton, &QPushButton::clicked, this, &ResultWindow::retryRequested);

    m_backButton = new QPushButton(this);
    m_backButton->setIcon(QIcon(":/images/back.png"));
    m_backButton->setIconSize(QSize(60, 60));
    m_backButton->setFlat(true);
    m_backButton->setGeometry(390, 300, 60, 60);
    connect(m_backButton, &QPushButton::clicked, this, &ResultWindow::backToMenuRequested);
}

// levelselectwindow.cpp
#include "levelselectwindow.h"
#include "gamewindow.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPixmap>
#include <QDebug>
#include <QFile>
#include <QTextStream>

LevelSelectWindow::LevelSelectWindow(QWidget *parent)
    : QMainWindow(parent) {
    setFixedSize(1280, 720);
    readDataFromFile();  // 读取文件数据
    setupUI();
}

void LevelSelectWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QPixmap bg(":/images/choose.png");
    painter.drawPixmap(rect(), bg);
}

void LevelSelectWindow::setupUI() {
    QWidget *scrollContent = new QWidget;

    // 垂直布局（保持原始边距）
    QVBoxLayout *vLayout = new QVBoxLayout(scrollContent);
    vLayout->setContentsMargins(0, 30, 0, 0);  // 原始顶部偏移30px
    vLayout->setSpacing(0);

    // 水平布局（保持原始边距）
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(60, 0, 0, 0); // 原始左偏移60px
    hLayout->setSpacing(10);                   // 原始按钮间距10px

    // 创建关卡按钮（保持原始尺寸）
    for (int i = 1; i <= 5; ++i) {
        QPushButton *btn = new QPushButton;
        btn->setFixedSize(128, 224); // 原始按钮尺寸
        btn->setIcon(QIcon(QString(":/images/level%1.png").arg(i)));
        btn->setIconSize(btn->size());
        btn->setStyleSheet("QPushButton { border: none; }"); // 原始样式

        connect(btn, &QPushButton::clicked, [i, this]() {
            GameWindow *game = new GameWindow(i, this);
            connect(game, &GameWindow::closed, this, [=](){
                this->show();
                game->deleteLater();
            });
            this->hide();
            game->show();
        });

        hLayout->addWidget(btn);
    }

    vLayout->addLayout(hLayout);

    // 滚动区域设置（保持原始定位逻辑）
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn); // 原始滚动条设置
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFixedSize(1100, 600);
    scrollArea->move(         // 原始动态定位公式
        width() - scrollArea->width() - 30,
        height() - scrollArea->height() - 30
        );
    scrollArea->setStyleSheet("background: transparent; border: none;"); // 原始透明样式

    // 添加金币和信件图标
    m_coinIcon = new QLabel(this);
    m_coinIcon->setPixmap(QPixmap(":/images/coin.png").scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_coinIcon->setGeometry(20, 20, 40, 40);

    m_coinText = new QLabel(QString("× %1").arg(m_coinCount), this);
    m_coinText->setStyleSheet("color: white; font: bold 20px;");
    m_coinText->setGeometry(70, 20, 60, 40);
    m_coinText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_letterIcon = new QLabel(this);
    m_letterIcon->setPixmap(QPixmap(":/images/letter.png").scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_letterIcon->setGeometry(20, 70, 40, 40);

    m_letterText = new QLabel(QString("× %1").arg(m_letterCount), this);
    m_letterText->setStyleSheet("color: white; font: bold 20px;");
    m_letterText->setGeometry(70, 70, 60, 40);
    m_letterText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

void LevelSelectWindow::readDataFromFile() {
    QFile file("data.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in >> m_coinCount >> m_letterCount;
        file.close();
    }
}

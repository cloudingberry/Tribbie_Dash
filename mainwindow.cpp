#include "mainwindow.h"
#include "levelselectwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QPainter>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setFixedSize(1280, 720);
    setupUI();
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPixmap bg(":/images/Main_background.jpg");
    painter.drawPixmap(this->rect(), bg);  // 拉伸铺满整个窗口
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);

    QPushButton *btn = createCustomButton();

    // 左下角布局
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(btn);
    hLayout->addStretch();
    hLayout->setContentsMargins(30, 0, 0, 30);

    QVBoxLayout *vLayout = new QVBoxLayout(centralWidget);
    vLayout->addStretch();
    vLayout->addLayout(hLayout);

    connect(btn, &QPushButton::clicked, [=]() {
        auto levelWindow = new LevelSelectWindow();
        levelWindow->show();
        this->hide();
    });


    setCentralWidget(centralWidget);
}

QPushButton* MainWindow::createCustomButton() {
    QPushButton *btn = new QPushButton();
    // 使用qrc路径加载按钮图标
    btn->setIcon(QIcon(":/images/choose_btn.png"));
    btn->setIconSize(QSize(90, 90));
    btn->setFixedSize(110, 110);
    btn->setStyleSheet(
        "QPushButton { border-radius: 55px; background: rgba(200,200,200,0.7); }"
        "QPushButton:hover { background: rgba(255,255,255,0.9); }"
        "QPushButton:pressed { padding-top: 2px; }"
        );
    return btn;
}

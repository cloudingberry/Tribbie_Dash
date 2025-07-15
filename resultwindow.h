#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class ResultWindow : public QWidget {
    Q_OBJECT

public:
    explicit ResultWindow(bool success, int coinCount, int letterCount, int lifeCount, QWidget* parent = nullptr);

signals:
    void retryRequested();
    void backToMenuRequested();

private:
    QLabel* m_background;
    QLabel* m_coinLabel;
    QLabel* m_letterLabel;
    QLabel* m_lifeLabel;

    QPushButton* m_retryButton;
    QPushButton* m_backButton;

    void setupUI(bool success, int coins, int letters, int lives);
};

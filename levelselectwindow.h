// levelselectwindow.h
#pragma once
#include <QMainWindow>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>

class LevelSelectWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit LevelSelectWindow(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
    void readDataFromFile();  // 读取文件数据

    QLabel* m_coinIcon;
    QLabel* m_coinText;
    QLabel* m_letterIcon;
    QLabel* m_letterText;
    int m_coinCount = 0;
    int m_letterCount = 0;
};

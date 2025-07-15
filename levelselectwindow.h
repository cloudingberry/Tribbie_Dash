#pragma once
#include <QMainWindow>
#include <QScrollArea>
#include <QPushButton>


class LevelSelectWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit LevelSelectWindow(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
};

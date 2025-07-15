#pragma once
#include <QMainWindow>
#include <QtMultimedia/QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>

class VideoWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit VideoWindow(QWidget *parent = nullptr);
    void playIntroVideo();

signals:
    void videoFinished();
private:
    QMediaPlayer *player;
    QVideoWidget *videoWidget;
};

#include "videowindow.h"

VideoWindow::VideoWindow(QWidget *parent) : QMainWindow(parent) {
    setFixedSize(1280, 720);
    player = new QMediaPlayer(this);
    videoWidget = new QVideoWidget(this);
    player->setVideoOutput(videoWidget);
    setCentralWidget(videoWidget);

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::EndOfMedia) {
            emit videoFinished();
            close();
        }
    });

}

void VideoWindow::playIntroVideo() {
    // 使用qrc路径访问视频资源
    QAudioOutput *audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(1.0);
    player->setSource(QUrl("qrc:/video/opening_animation.mp4"));
    player->play();
}

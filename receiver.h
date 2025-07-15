#pragma once
#include "gameitem.h"
#include <QSoundEffect>

class Receiver : public GameItem {
    Q_OBJECT
public:
    explicit Receiver(QWidget* parent, Track track);

public slots:
    void onCollideWithPlayer();

signals:
    void goalReached();  // 播放完音效后触发
};

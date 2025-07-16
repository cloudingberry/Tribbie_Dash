#pragma once
#include "gameitem.h"
#include "qgraphicseffect.h"
#include <QPropertyAnimation>
#include <QSoundEffect>
class spearitem: public GameItem {
    Q_OBJECT
public:
    explicit spearitem(QWidget* parent, Track track);

    // 长矛特有功能
    void activateThrust();  // 激活冲刺效果
    bool isActive() const;  // 是否处于冲刺状态

public slots:
    void onCollideWithPlayer();

private:
    QSoundEffect* m_spearSound;       // 冲刺音效
    QSoundEffect m_shieldBreakSound; // 盾牌碎裂音效
    bool m_isActive = false;
};

#include "character.h"
#include <QTimer>

Character::Character(QWidget* parent) : QLabel(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);

    m_spearTimer = new QTimer(this);
    m_spearTimer->setSingleShot(true);
    connect(m_spearTimer, &QTimer::timeout, this, &Character::endSpearMode);

    m_dashTimer = new QTimer(this);
    m_dashTimer->setSingleShot(true);
    connect(m_dashTimer, &QTimer::timeout, this, &Character::endDash);
}

void Character::init(Type type, const QList<int>& trackYPositions, int yOffset) {
    m_trackYPositions = trackYPositions;
    m_yOffset = yOffset;
    switchType(type);  // 调用 switchType 设置角色初始类型
}

void Character::switchType(Type type) {
    if (type == m_currentType) return;

    m_isJumping = false;        // 切换角色后重置跳跃状态
    m_verticalVelocity = 0;     //             速度
    m_currentType = type;       //             当前角色
    int trackIndex = type - 1;  // 由角色编号计算新轨道编号

    QString imgPath;
    if (type == AN) imgPath = ":/images/an.png";
    else if (type == NING) imgPath = ":/images/ning.png";
    else imgPath = ":/images/bao.png";

    QPixmap pixmap(imgPath);
    if (!pixmap.isNull()) {
        pixmap = pixmap.scaled(CHARACTER_SIZE, CHARACTER_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setPixmap(pixmap);
    }

    // 初始位置
    setGeometry(
        parentWidget()->width() / 2 - CHARACTER_SIZE / 2,
        m_trackYPositions[trackIndex] - m_yOffset,
        CHARACTER_SIZE, CHARACTER_SIZE
        );
}

void Character::processJump(qreal deltaSec) {
    Q_UNUSED(deltaSec);

    if (m_currentType == AN) return;

    if (m_isJumping) {
        m_verticalVelocity += GRAVITY;
        int newY = y() + static_cast<int>(m_verticalVelocity);

        int targetY = m_trackYPositions[m_currentType - 1] - m_yOffset;

        if (newY <= m_trackYPositions[0] - m_yOffset) {
            newY = m_trackYPositions[0] - m_yOffset;
            m_verticalVelocity = 0;
        } else if (newY >= targetY) {
            newY = targetY;
            m_verticalVelocity = 0;
            m_isJumping = false;
            m_canDoubleJump = true;
        }

        move(x(), newY);
    }
}

void Character::startJump() {
    if (m_currentType == AN) return;

    if (!m_isJumping) {
        m_verticalVelocity = FIRST_JUMP_FORCE;
        m_isJumping = true;
        m_canDoubleJump = true;
        playJumpSound();
    } else if (m_canDoubleJump && m_currentType == BAO) {
        m_verticalVelocity = SECOND_JUMP_FORCE;
        m_canDoubleJump = false;
        playJumpSound();
    }

    m_jumpTimer.start();
    emit jumped();
}

QRect Character::hitBox() const {
    return geometry().adjusted(25, 25, -25, -25);
}

void Character::playJumpSound() {
    QSoundEffect* sound = new QSoundEffect(this);
    sound->setSource(QUrl("qrc:/sound/jump.wav"));
    sound->setVolume(1.0);
    sound->play();
}




void Character::activateSpearMode(int durationSec) {
    m_inSpearMode = true;
    m_spearTimer->start(durationSec * 1000);
    emit spearModeChanged(true, durationSec);
}

void Character::triggerDash() {
    if (!m_inSpearMode || m_isDashing) return;
    m_isDashing = true;
    QSoundEffect* sound = new QSoundEffect(this);
    sound->setSource(QUrl("qrc:/sound/Spear_Dash.WAV"));
    sound->setVolume(1.0);
    sound->play();
    emit dashStarted();
    m_dashTimer->start(500); // 0.5秒冲刺
}

void Character::endSpearMode() {
    m_inSpearMode = false;
    emit spearModeChanged(false, 0);
}

void Character::endDash() {
    m_isDashing = false;
    emit dashEnded();
}


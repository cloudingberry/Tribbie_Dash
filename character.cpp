#include "character.h"

Character::Character(QWidget* parent)
    : QLabel(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void Character::init(Type type, const QList<int>& trackYPositions, int yOffset) {
    m_trackYPositions = trackYPositions;
    m_yOffset = yOffset;
    switchType(type);
}

void Character::switchType(Type type) {
    if (type == m_currentType) return;

    m_isJumping = false;
    m_verticalVelocity = 0;

    m_currentType = type;
    int trackIndex = type - 1;

    QString imgPath;
    if (type == AN) imgPath = ":/images/an.png";
    else if (type == NING) imgPath = ":/images/ning.png";
    else imgPath = ":/images/bao.png";

    QPixmap pixmap(imgPath);
    if (!pixmap.isNull()) {
        pixmap = pixmap.scaled(CHARACTER_SIZE, CHARACTER_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setPixmap(pixmap);
    }

    setGeometry(
        parentWidget()->width() / 2 - CHARACTER_SIZE / 2,
        m_trackYPositions[trackIndex] - m_yOffset,
        CHARACTER_SIZE, CHARACTER_SIZE
        );
}

void Character::processJump(qreal deltaSec) {
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
    return geometry().adjusted(30, 30, -30, -30);
}

void Character::playJumpSound() {
    QSoundEffect* sound = new QSoundEffect(this);
    sound->setSource(QUrl("qrc:/sound/jump.wav"));
    sound->setVolume(0.8);
    sound->play();
}

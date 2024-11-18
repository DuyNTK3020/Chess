#include "player.h"

// Constructor
Player::Player()
    : name(""), elo(0), status("")
{
}

Player::Player(const QString &name, int elo, const QString status)
    : name(name), elo(elo), status(status)
{
}

// Getters
QString Player::getName() const {
    return name;
}

int Player::getElo() const {
    return elo;
}

QString Player::getStatus() const {
    return status;
}


// Setters
void Player::setName(const QString &name) {
    this->name = name;
}

void Player::setElo(int elo) {
    this->elo = elo;
}

void Player::setStatus(const QString &status) {
    this->status = status;
}

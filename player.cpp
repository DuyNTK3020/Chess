#include "player.h"
#include <QJsonObject>

// Constructor
Player::Player()
    : name(""), username(""), elo(0), status("")
{
}

Player::Player(const QString &name, const QString &username, int elo, const QString status)
    : name(name), username(username), elo(elo), status(status)
{
}

Player::Player(const QJsonObject &jsonObj) {
    name = jsonObj["name"].toString();
    username = jsonObj["username"].toString();
    elo = jsonObj["elo"].toInt();
    status = jsonObj["state"].toString();
}


// Getters
QString Player::getName() const {
    return name;
}

QString Player::getUsername() const {
    return username;
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

void Player::setUsername(const QString &username) {
    this->username = username;
}

void Player::setElo(int elo) {
    this->elo = elo;
}

void Player::setStatus(const QString &status) {
    this->status = status;
}

#include "user.h"

// Constructor
User::User()
    : username(""), name(""), password(""), elo(0)
{
}

User::User(const QString &username, const QString &name, const QString &password, int elo)
    : username(username), name(name), password(password), elo(elo)
{
}

// Getters
QString User::getUsername() const {
    return username;
}

QString User::getName() const {
    return name;
}

QString User::getPassword() const {
    return password;
}

int User::getElo() const {
    return elo;
}

// Setters
void User::setUsername(const QString &username) {
    this->username = username;
}

void User::setName(const QString &name) {
    this->name = name;
}

void User::setPassword(const QString &password) {
    this->password = password;
}

void User::setElo(int elo) {
    this->elo = elo;
}

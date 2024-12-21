#ifndef PLAYER_H
#define PLAYER_H

#include <QString>
#include <QJsonObject>

class Player {
public:
    // Constructor
    Player();
    Player(const QString &name, const QString &username, int elo, const QString status);
    Player(const QJsonObject &jsonObj);

    // Getter và Setter
    QString getName() const;
    void setName(const QString &name);

    QString getUsername() const;
    void setUsername(const QString &username);

    int getElo() const;
    void setElo(int elo);

    QString getStatus() const;
    void setStatus(const QString &status);

private:
    QString name;
    QString username;
    int elo;
    QString status;
};

#endif // PLAYER_H

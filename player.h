#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

class Player {
public:
    // Constructor
    Player();
    Player(const QString &name, int elo, const QString status);

    // Getter và Setter
    QString getName() const;
    void setName(const QString &name);

    int getElo() const;
    void setElo(int elo);

    QString getStatus() const;
    void setStatus(const QString &status);

private:
    QString name;
    int elo;
    QString status;
};

#endif // PLAYER_H

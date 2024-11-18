#ifndef USER_H
#define USER_H

#include <QString>

class User
{
public:
    // Constructor
    User();
    User(const QString &username, const QString &name, const QString &password, int elo);

    // Getters
    QString getUsername() const;
    QString getName() const;
    QString getPassword() const;
    int getElo() const;

    // Setters
    void setUsername(const QString &username);
    void setName(const QString &name);
    void setPassword(const QString &password);
    void setElo(int elo);

private:
    QString username;
    QString name;
    QString password;
    int elo;
};

#endif // USER_H

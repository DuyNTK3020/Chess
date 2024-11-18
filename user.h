#ifndef USER_H
#define USER_H

#include <QString>

class User
{
public:
    // Constructor
    User();
    User(const QString &username, const QString &name, const QString &password, int elo, const QString token);

    // Getters
    QString getUsername() const;
    QString getName() const;
    QString getPassword() const;
    int getElo() const;
    QString getToken() const;

    // Setters
    void setUsername(const QString &username);
    void setName(const QString &name);
    void setPassword(const QString &password);
    void setElo(int elo);
    void setToken(const QString &token);

private:
    QString username;
    QString name;
    QString password;
    int elo;
    QString token;
};

#endif // USER_H

#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include <QTcpSocket>

class ClientManager : public QObject
{
    Q_OBJECT
public:
    explicit ClientManager(QObject *parent = nullptr);
    void connectToServer(const QString &host, int port);
    void sendMove(const QString &moveData);
    void sendRegisterRequest(const QString &name, const QString &username, const QString &password);
    void sendLoginRequest(const QString &username, const QString &password);
    void sendConnectRequest(const QString &token);
    void sendFindMatchRequest(const QString &username);
    void sendCreateRoomRequest(const QString &username);

signals:
    void moveReceived(const QString &moveData);
    void registerResult(const QString &status, const QString &message);
    void loginResult(const QString &status, const QString &message, const QString &name, int elo, const QString &token);
    void connectionResult(const QString &status, const QString &message);
    void findMatchResult(const QString &status, const QString &room, const QString &competitor, const QString &role);
    void createRoomResult(const QString &status, const QString &room);
private slots:
    void onReadyRead();

private:
    QTcpSocket *socket;
};

#endif // CLIENTMANAGER_H

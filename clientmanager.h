#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include <QTcpSocket>

#include "player.h"

class ClientManager : public QObject
{
    Q_OBJECT
public:
    explicit ClientManager(QObject *parent = nullptr);
    void connectToServer(const QString &host, int port);
    void sendMove(const QString &game_id, const QString &username,int oldRow, int oldCol, int newRow, int newCol);
    void sendRegisterRequest(const QString &name, const QString &username, const QString &password);
    void sendLoginRequest(const QString &username, const QString &password);
    void sendConnectRequest(const QString &token);
    void sendFindMatchRequest(const QString &username);
    void sendCreateRoomRequest(const QString &username);
    void sendUpdateProfileRequest(const QString &username, const QString &name);
    void sendChangePasswordRequest(const QString &username, const QString &password);
    void sendGetListPlayerRequest(const QString &username);
    void sendInvitePlayerRequest(const QString &username, const QString &name, const QString &invitePlayer);
    void sendRespondInviteRequest(const QString &status, const QString &username, const QString &invite_player);

signals:
    void moveReceived(const QString &moveData);
    void registerResult(const QString &status, const QString &message);
    void loginResult(const QString &status, const QString &message, const QString &name, int elo, const QString &token);
    void connectionResult(const QString &status, const QString &message);
    void findMatchResult(const QString &status, const QString &message, const QString &opponent, const QString &match_id, const QString &role);
    void createRoomResult(const QString &status, const QString &message);
    void updateProfileResult(const QString &status, const QString &message);
    void changePasswordResult(const QString &status, const QString &message);
    void getListPlayerResult(const QString &status, const QString &message, const QList<Player> &players);
    void invitePlayerResult(const QString &username, const QString &name);
    void respondInviteResult(const QString &status, const QString &message, Player &player);

private slots:
    void onReadyRead();

private:
    QTcpSocket *socket;
};

#endif // CLIENTMANAGER_H

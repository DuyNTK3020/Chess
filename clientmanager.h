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

signals:
    void moveReceived(const QString &moveData);
    void registerResponseReceived(const QString &status, const QString &message);
    void loginResult(bool success);
private slots:
    void onReadyRead();

private:
    QTcpSocket *socket;
};

#endif // CLIENTMANAGER_H

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

signals:
    void moveReceived(const QString &moveData);

private slots:
    void onReadyRead();

private:
    QTcpSocket *socket;
};

#endif // CLIENTMANAGER_H

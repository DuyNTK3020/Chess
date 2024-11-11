#include "clientmanager.h"
#include <QDebug>

ClientManager::ClientManager(QObject *parent) : QObject(parent), socket(new QTcpSocket(this))
{
    connect(socket, &QTcpSocket::readyRead, this, &ClientManager::onReadyRead);
}

void ClientManager::connectToServer(const QString &host, int port)
{
    socket->connectToHost(host, port);
    if (!socket->waitForConnected(3000)) {
        qDebug() << "Connection failed!";
    } else {
        qDebug() << "Connected to server!";
        sendMove("Hello server chess :))!\n");
    }
}

void ClientManager::sendMove(const QString &moveData)
{
    if (socket && socket->isWritable()) {
        socket->write(moveData.toUtf8());
        socket->flush();
    } else {
        qDebug() << "Socket not writable!";
    }
}

void ClientManager::onReadyRead()
{
    QString moveData = QString::fromUtf8(socket->readAll());
    emit moveReceived(moveData);
}

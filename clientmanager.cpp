#include "clientmanager.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
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

void ClientManager::sendRegisterRequest(const QString &username, const QString &password)
{
    // Tạo đối tượng JSON để gửi
    QJsonObject json;
    json["type"] = "register";  // Loại yêu cầu là đăng ký
    json["username"] = username;  // Tên người dùng
    json["password"] = password;  // Mật khẩu người dùng

    // Chuyển đổi đối tượng JSON thành chuỗi byte (UTF-8)
    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);  // Sử dụng định dạng gọn gàng

    // Kiểm tra xem socket có thể ghi dữ liệu không
    if (socket && socket->isWritable()) {
        // Gửi dữ liệu qua socket
        socket->write(data);
        socket->flush();  // Đảm bảo dữ liệu được gửi ngay lập tức
        qDebug() << "Sent register request:" << data;
    } else {
        qDebug() << "Socket not writable!";
    }
}

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

    qDebug() << moveData;

    QJsonDocument doc = QJsonDocument::fromJson(moveData.toUtf8());
    if (doc.isObject()) {
        QJsonObject jsonObj = doc.object();
        if (jsonObj.contains("type") && jsonObj["type"].toString() == "register_ack") {
            QString status = jsonObj["status"].toString();
            QString message = jsonObj["message"].toString();
            emit registerResponseReceived(status, message);
        }
        if (jsonObj.contains("type") && jsonObj["type"].toString() == "login_ack") {
            QString status = jsonObj["status"].toString();
            if (status == "success") {
                emit loginResult(true);  // Đăng nhập thành công
            } else {
                emit loginResult(false);  // Đăng nhập thất bại
            }
        }
    }
}

void ClientManager::sendRegisterRequest(const QString &name, const QString &username, const QString &password)
{
    // Tạo đối tượng JSON để gửi
    QJsonObject json;
    json["type"] = "register";  // Loại yêu cầu là đăng ký
    json["name"] = name;          // Tên người dùng
    json["username"] = username;  // Tên tài khoản người dùng
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

void ClientManager::sendLoginRequest(const QString &username, const QString &password)
{
    QJsonObject json;
    json["type"] = "login";
    json["username"] = username;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    if (socket && socket->isWritable()) {
        socket->write(data);
        socket->flush();
        qDebug() << "Sent login request:" << data;
    } else {
        qDebug() << "Socket not writable!";
    }
}

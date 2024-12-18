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
            // qDebug() << "Register " << status << ": " << message;
            emit registerResult(status, message);
        }
        if (jsonObj.contains("type") && jsonObj["type"].toString() == "login_ack") {
            QString status = jsonObj["status"].toString();
            QString message = jsonObj["message"].toString();
            QString name = jsonObj["name"].toString();
            int elo = jsonObj["elo"].toString().toInt();
            QString token = jsonObj["token"].toString();
            // qDebug() << "Login " << status << ": " << name << " " << elo;
            emit loginResult(status, message, name, elo, token);  // Đăng nhập thành công
        }
        if (jsonObj.contains("type") && jsonObj["type"].toString() == "connect_ack") {
            QString status = jsonObj["status"].toString();
            QString message = jsonObj["message"].toString();
            // qDebug() << "Connection " << status << ": " << message;
            emit connectionResult(status, message); // Phát tín hiệu kết nối thành công
        }
        if (jsonObj.contains("type") && jsonObj["type"].toString() == "find_match_ack") {
            QString status = jsonObj["status"].toString();
            QString message = jsonObj["message"].toString();
            QString opponent = jsonObj["opponent"].toString();
            QString match_id = jsonObj["match_id"].toString();
            QString role = jsonObj["role"].toString();
            if (status == "success") {
                emit findMatchResult(status, message, opponent, match_id, role);
            } else {
                emit findMatchResult(status, message, "", "", "");
            }
        }
        if (jsonObj.contains("type") && jsonObj["type"].toString() == "create_room_ack") {
            QString status = jsonObj["status"].toString();
            QString message = jsonObj["message"].toString();
            emit createRoomResult(status, message);
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

void ClientManager::sendConnectRequest(const QString &token)
{
    QJsonObject json;
    json["type"] = "connect";
    json["token"] = token;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    if (socket && socket->isWritable()) {
        socket->write(data);
        socket->flush();
        qDebug() << "Sent connect request:" << data;
    } else {
        qDebug() << "Socket not writable!";
    }
}

void ClientManager::sendFindMatchRequest(const QString &username)
{
    QJsonObject json;
    json["type"] = "find_match";
    json["username"] = username;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    if (socket && socket->isWritable()) {
        socket->write(data);
        socket->flush();
        qDebug() << "Sent connect request:" << data;
    } else {
        qDebug() << "Socket not writable!";
    }
}

void ClientManager::sendCreateRoomRequest(const QString &username)
{
    QJsonObject json;
    json["type"] = "create_room";
    json["username"] = username;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    if (socket && socket->isWritable()) {
        socket->write(data);
        socket->flush();
        qDebug() << "Sent connect request:" << data;
    } else {
        qDebug() << "Socket not writable!";
    }
}

void ClientManager::sendUpdateProfileRequest(const QString &username, const QString &name) {
    QJsonObject json;
    json["type"] = "update_profile";
    json["username"] = username;
    json["name"] = name;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    if (socket && socket->isWritable()) {
        socket->write(data);
        socket->flush();
        qDebug() << "Sent connect request:" << data;
    } else {
        qDebug() << "Socket not writable!";
    }
}

void ClientManager::sendChangePasswordRequest(const QString &username, const QString &password) {
    QJsonObject json;
    json["type"] = "change_password";
    json["username"] = username;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    if (socket && socket->isWritable()) {
        socket->write(data);
        socket->flush();
        qDebug() << "Sent connect request:" << data;
    } else {
        qDebug() << "Socket not writable!";
    }
}

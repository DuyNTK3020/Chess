#include "clientmanager.h"
#include "player.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

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

void ClientManager::sendMove(const QString &game_id, const QString &username,
                             int oldRow, int oldCol, int newRow, int newCol)
{
    // Tạo đối tượng JSON để gửi
    QJsonObject json;
    json["type"] = "move";  // Loại yêu cầu
    json["game_id"] = game_id;  // ID của game
    json["username"] = username;  // Tên tài khoản người dùng
    json["old_row"] = oldRow;  // Hàng cũ
    json["old_col"] = oldCol;  // Cột cũ
    json["new_row"] = newRow;  // Hàng mới
    json["new_col"] = newCol;  // Cột mới

    // Chuyển đổi đối tượng JSON thành chuỗi byte (UTF-8)
    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);  // Sử dụng định dạng gọn gàng

    // Kiểm tra xem socket có thể ghi dữ liệu không
    if (socket && socket->isWritable()) {
        // Gửi dữ liệu qua socket
        socket->write(data);
        socket->flush();  // Đảm bảo dữ liệu được gửi ngay lập tức
        qDebug() << "Sent move request:" << data;
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
            int elo = jsonObj["elo"].toInt();
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
            QString room_id = jsonObj["room_id"].toString();
            emit createRoomResult(status, message, room_id);
        }
        if (jsonObj.contains("type") && jsonObj["type"].toString() == "update_profile_ack") {
            QString status = jsonObj["status"].toString();
            QString message = jsonObj["message"].toString();
            emit updateProfileResult(status, message);
        }
        if (jsonObj.contains("type") && jsonObj["type"].toString() == "get_list_player_ack") {
            QString status = jsonObj["status"].toString();
            QString message = jsonObj["message"].toString();

            QList<Player> players;
            if (jsonObj.contains("players") && jsonObj["players"].isArray()) {
                QJsonArray playersArray = jsonObj["players"].toArray();
                for (const QJsonValue &value : playersArray) {
                    if (value.isObject()) {
                        QJsonObject playerObj = value.toObject();
                        players.append(Player(playerObj)); // Thêm đối tượng Player
                    }
                }
            }
            emit getListPlayerResult(status, message, players);
        }
        if (jsonObj.contains("type") && jsonObj["type"].toString() == "invite_player_ack") {
            QString username = jsonObj["username"].toString();
            QString name = jsonObj["name"].toString();
            emit invitePlayerResult(username, name);
        }
        if (jsonObj.contains("type") && jsonObj["type"].toString() == "respond_invite_ack") {
            QString status = jsonObj["status"].toString();
            QString message = jsonObj["message"].toString();
            Player user;
            if (jsonObj.contains("user") && jsonObj["user"].isObject()) {
                QJsonObject userObj = jsonObj["user"].toObject();
                user = Player(userObj);
            }
            emit respondInviteResult(status, message, user);
        }
        if (jsonObj.contains("type") && jsonObj["type"].toString() == "invitation_ack") {
            QString status = jsonObj["status"].toString();
            QString message = jsonObj["message"].toString();
            Player user;
            if (jsonObj.contains("user") && jsonObj["user"].isObject()) {
                QJsonObject userObj = jsonObj["user"].toObject();
                user = Player(userObj);
            }
            emit invitationResult(status, message, user);
        }
        if (jsonObj.contains("type") && jsonObj["type"].toString() == "start_game_ack") {
            QString status = jsonObj["status"].toString();
            QString message = jsonObj["message"].toString();
            QString opponent = jsonObj["opponent"].toString();
            QString match_id = jsonObj["match_id"].toString();
            QString role = jsonObj["role"].toString();
            if (status == "success") {
                emit startGameResult(status, message, opponent, match_id, role);
            } else {
                emit startGameResult(status, message, "", "", "");
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

void ClientManager::sendConnectRequest(const QString &token, const QString &username)
{
    QJsonObject json;
    json["type"] = "connect";
    json["token"] = token;
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

void ClientManager::sendGetListPlayerRequest(const QString &username) {
    QJsonObject json;
    json["type"] = "get_list_player";
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

void ClientManager::sendInvitePlayerRequest(const QString &username, const QString &name, const QString &invitePlayer) {
    QJsonObject json;
    json["type"] = "invite_player";
    json["username"] = username;
    json["name"] = name;
    json["invite_player"] = invitePlayer;

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

void ClientManager::sendRespondInviteRequest(const QString &status, const QString &username, const QString &invite_player) {
    QJsonObject json;
    json["type"] = "respond_invite";
    json["status"] = status;
    json["username"] = username;
    json["invite_player"] = invite_player;

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

void ClientManager::sendOutRoomRequest(const QString &username, const QString &opponent, const QString &room_id) {
    QJsonObject json;
    json["type"] = "out_room";
    json["username"] = username;
    json["opponent"] = opponent;
    json["room_id"] = room_id;

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

void ClientManager::sendStartGameRequest(const QString &username1, const QString &username2) {
    QJsonObject json;
    json["type"] = "out_room";
    json["username1"] = username1;
    json["username2"] = username2;

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

#include <QApplication>
#include "game.h"
#include "clientmanager.h"
#include "user.h"

Game *game;
ClientManager *clientManager;
User *user;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Tạo user
    user = new User();

    // Khởi tạo ClientManager và kết nối tới server
    clientManager = new ClientManager();
    clientManager->connectToServer("127.0.0.1", 5500); // Đặt IP và cổng server

    // Khởi tạo game
    game = new Game();
    game->show();
    // game->displayLogin();
    game->displayRoom("1000");

    // Kết nối tín hiệu `moveMade` của tất cả các `ChessPiece` với `ClientManager`
    // for (ChessPiece* piece : game->getAllChessPieces()) { // Giả sử bạn có phương thức để lấy tất cả các quân cờ
    //     QObject::connect(piece, &ChessPiece::moveMade, clientManager, &ClientManager::sendMove);
    // }
    return a.exec();
}

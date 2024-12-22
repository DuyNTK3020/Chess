#include <QApplication>
#include <QList>
#include "game.h"
#include "clientmanager.h"
#include "user.h"
#include "player.h"

Game *game;
ClientManager *clientManager;
User *user;
QList<Player*> players;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Tạo user
    user = new User();

    // Khởi tạo ClientManager và kết nối tới server
    clientManager = new ClientManager();
    clientManager->connectToServer("127.0.0.1", 5500); // Đặt IP và cổng server

    players.append(new Player("Player1","Player1", 1200, "Online"));
    players.append(new Player("Player2","Player1", 1500, "In Match"));
    players.append(new Player("Player3","Player1", 1000, "In Match"));
    players.append(new Player("Player4","Player1", 1200, "Online"));

    // Khởi tạo game
    game = new Game();
    game->show();
    game->displayLogin();
    // game->displayRoom(nullptr);
    // game->displayMenu("");

    return a.exec();
}

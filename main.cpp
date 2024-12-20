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

    players.append(new Player("Player1", 1200, "Online"));
    players.append(new Player("Player2", 1500, "In game"));
    players.append(new Player("Player3", 1000, "In game"));
    players.append(new Player("Player4", 1200, "Online"));
    players.append(new Player("Player5", 1500, "In game"));
    players.append(new Player("Player6", 1000, "In game"));
    players.append(new Player("Player7", 1200, "Online"));
    players.append(new Player("Player8", 1500, "In game"));
    players.append(new Player("Player9", 1000, "In game"));
    players.append(new Player("1Player1", 1200, "Online"));
    players.append(new Player("aPlayer2", 1500, "In game"));
    players.append(new Player("Palayer3", 1000, "In game"));
    players.append(new Player("Plaayer4", 1200, "Online"));
    players.append(new Player("Plaaayer5", 1500, "In game"));
    players.append(new Player("Playear6", 1000, "In game"));
    players.append(new Player("Playera7", 1200, "Online"));
    players.append(new Player("Player8a", 1500, "In game"));
    players.append(new Player("Player9a", 1000, "In game"));

    // Khởi tạo game
    game = new Game();
    game->show();
    game->displayLogin();

    return a.exec();
}

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
    //game->moveChessPiece(1,2,1,3);

    ChessBox *oldBox = new ChessBox();
    ChessBox *newBox = new ChessBox();



    oldBox = game->collection[1][0];
    newBox = game->collection[1][1];

    if(!game->collection[1][0]){
        qDebug()<< "Null vai lon";
    }

    if (!oldBox || !newBox) {
        qDebug() << "Ô cũ hoặc ô mới không hợp lệ!";
        //return;
    }

    qDebug() << "Ô cũ hoặc ô mới có hợp lệ!";

    if (!oldBox->currentPiece) {
        qDebug() << "currentPiece là null.";
    } else {
        qDebug() << "currentPiece không phải là null.";
    }



    ChessPiece *piece = oldBox->currentPiece;
    //qDebug() << "oldBox:" << oldBox << ", currentPiece:" << oldBox->currentPiece;

   // qDebug() << piece->getSide();

    //if (!piece) {
    //    qDebug() << "Không có quân cờ ở vị trí (" << oldCol << "," << oldRow << ")";
    //    return;
    //}


    //oldBox->setHasChessPiece(false);
    //oldBox->currentPiece = NULL;

    //newBox->setHasChessPiece(true);
    //newBox->currentPiece = piece;

    //piece->setCurrentBox(newBox);



    //game->displayProfile();S


    return a.exec();
}

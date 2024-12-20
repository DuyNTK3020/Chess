#ifndef GAME_H
#define GAME_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include <chessboard.h>
#include "chesspiece.h"
class Game:public QGraphicsView
{
    Q_OBJECT
public:
    //Constructors
    Game(QWidget *parent = 0);

    //public Methods
    void setBackground();
    void drawDeadHolder(int x, int y, QColor color);
    void drawChessBoard(QString role);
    void displayDeadWhite();
    void displayDeadBlack();
    void placeInDeadPlace(ChessPiece *piece);

    //Scene Related
    void addToScene(QGraphicsItem *item);
    void removeFromScene(QGraphicsItem *item);

    //getters/setters
    ChessPiece *pieceToMove;

    QString getTurn() ;
    void setTurn( QString value);
    void changeTurn();

    QList<ChessPiece *> getAllChessPieces();


    ChessBox *collection[8][8];
    QGraphicsTextItem *check;
    QList <ChessPiece *> alivePiece;
    void displayLogin();
    void displayRegister();
    void displayWaitConnect();
    void displayMenu();
    void displayWaitFindMatch();
    void displayRoom();
    void displayProfile();

    void gameOver();
    void removeAll();
    void clearScene();
    static QString role;
    static QString match_id;




public slots:
    void start(const QString &status, const QString &match_id, const QString &opponent, const QString &role);

private:
    QGraphicsPixmapItem* background = nullptr;
    QGraphicsScene *gameScene;
    QList <ChessPiece *> whiteDead;
    QList <ChessPiece *> blackDead;
    QGraphicsRectItem * deadHolder;
    QString turn;
    ChessBoard *chess;
    QList <QGraphicsItem *> listG;
    QGraphicsTextItem * turnDisplay;


};

#endif // GAME_H

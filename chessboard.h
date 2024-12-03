#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <QGraphicsRectItem>
#include "chesspiece.h"
class ChessBoard
{
public:
    ChessBoard(QString role);

    //drawing public function
    void drawBoxes(int x, int y);

    void setUpWhite();
    void setUpBlack();

    void setUpWhite2();
    void setUpBlack2();

    void addChessPiece(QString role);
    void addChessPiece2();
    void reset();
private:
    QList <ChessPiece *> white;
    QList <ChessPiece *> black;



};

#endif // CHESSBOARD_H

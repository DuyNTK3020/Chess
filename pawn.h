#ifndef PAWN_H
#define PAWN_H

#include "chesspiece.h"
class Pawn:public ChessPiece
{
public:
    Pawn(QString team,QGraphicsItem *parent = 0);
    void setImage();
    void moves();

 private:
int firstrow;

};

#endif // PAWN_H

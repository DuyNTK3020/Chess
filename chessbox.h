#ifndef CHESSBOX_H
#define CHESSBOX_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "chesspiece.h"
#include <QGraphicsSceneMouseEvent>

class ChessPiece;
class ChessBox:public QGraphicsRectItem
{
public:

    //Constructor
    ChessBox(QGraphicsItem *parent=0);
    ~ChessBox();


    //public member function
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void setColor(QColor color);
    void placePiece(ChessPiece *piece);
    void placePiece2(ChessPiece *piece);



    void resetOriginalColor();
    void setOriginalColor(QColor value);

    bool getHasChessPiece();
    void setHasChessPiece(bool value,ChessPiece *piece = 0);

    void checkForCheck();
    void checkForCheck2();


    QString getChessPieceColor() ;
    void setChessPieceColor(QString value);
    void updateOpponentMove(ChessBox *oldBox, ChessBox *newBox);

    int rowLoc;
    int colLoc;

    static int count;

    QString getPosition();

    ChessPiece * currentPiece;

private:
    QBrush brush;
    QColor originalColor;
    bool hasChessPiece;
    QString chessPieceColor;


};

#endif // CHESSBOX_H

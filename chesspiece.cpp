#include "chesspiece.h"
#include "game.h"
#include <QDebug>
#include "king.h"

extern Game *game;
ChessPiece::ChessPiece(QString team, QGraphicsItem *parent):QGraphicsPixmapItem(parent)
{
    side = team;
    isPlaced = true;
    firstMove = true;
}

void ChessPiece::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //Deselect
    if(this == game->pieceToMove){
        game->pieceToMove->getCurrentBox()->resetOriginalColor();
        game->pieceToMove->decolor();
        game->pieceToMove = NULL;
        // qDebug() << game->pieceToMove->getCurrentBox()->getPosition();
        return;
    }
    //if it is already consumed or not the respective color's turn
    if((!getIsPlaced() )|| ( (game->getTurn() != this->getSide())&& (!game->pieceToMove)) )
        return;
    //selecting
    if(!game->pieceToMove){
        game->pieceToMove = this;
        game->pieceToMove->getCurrentBox()->setColor(Qt::red);
        game->pieceToMove->moves();
        qDebug() << game->pieceToMove->getCurrentBox()->getPosition();
    }
    //Consuming counterPart of the CHessBox
    else if(this->getSide() != game->pieceToMove->getSide()){
        this->getCurrentBox()->mousePressEvent(event);
        // qDebug() << game->pieceToMove->getCurrentBox()->getPosition();
        // QString moveData = game->pieceToMove->getCurrentBox()->getPosition();
        // qDebug() << moveData;
        // emit moveMade(moveData);
    }
}

void ChessPiece::setCurrentBox(ChessBox *box)
{

    currentBox = box;
}

ChessBox *ChessPiece::getCurrentBox()
{
    return currentBox;
}

QString ChessPiece::getSide()
{
    //qDebug() << "Đang lấy side của ChessPiece:" << side;
    return side;
}

void ChessPiece::setSide( QString value)
{
    side = value;
}

bool ChessPiece::getIsPlaced()
{
    return isPlaced;
}

void ChessPiece::setIsPlaced(bool value)
{
    isPlaced = value;

}
QList<ChessBox *> ChessPiece::moveLocation()
{
    return location;
}
void ChessPiece::decolor()
{
    for(size_t i = 0, n = location.size(); i < n;i++) {
         location[i]->resetOriginalColor();
    }
}

bool ChessPiece::boxSetting(ChessBox *box)
{
    if(box->getHasChessPiece()) {
        King *q = dynamic_cast<King*>(location.last()->currentPiece);
        if(q){
            box->setColor(Qt::blue);
        }
        else
            box->setColor(Qt::red);
        return true;
    }
    else
        location.last()->setColor(Qt::darkRed);
    return false;
}

void ChessPiece::moveChessPiece(int oldCol, int oldRow, int newCol, int newRow) {
    qDebug() << "Bắt đầu di chuyển từ (" << oldCol << "," << oldRow << ") đến (" << newCol << "," << newRow << ")";

    ChessBox *oldBox = game->collection[oldRow][oldCol];
    ChessBox *newBox = game->collection[newRow][newCol];

    if (!oldBox || !newBox) {
        qDebug() << "Ô cũ hoặc ô mới không hợp lệ!";
        return;
    }

    qDebug() << "Ô cũ hoặc ô mới có hợp lệ!";

    if (!oldBox->currentPiece) {
        qDebug() << "currentPiece là null.";
    } else {
        qDebug() << "currentPiece không phải là null.";
    }



    ChessPiece *piece = oldBox->currentPiece;
    //qDebug() << "oldBox:" << oldBox << ", currentPiece:" << oldBox->currentPiece;

    qDebug() << "Quân cờ tại (" << oldCol << "," << oldRow << ") là của bên: " << piece->getSide();

    //if (!piece) {
    //    qDebug() << "Không có quân cờ ở vị trí (" << oldCol << "," << oldRow << ")";
    //    return;
    //}




    qDebug() << "Di chuyển quân cờ thành công từ (" << oldCol << "," << oldRow << ") đến (" << newCol << "," << newRow << ")";
}




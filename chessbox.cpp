#include "chessbox.h"
#include "game.h"
#include <QDebug>
#include "king.h"
#include "user.h"
#include "clientmanager.h"


extern Game *game;
extern ClientManager *clientManager;
extern User *user;

ChessBox::ChessBox(QGraphicsItem *parent):QGraphicsRectItem(parent)
{
    //making the Square CHess Box
    setRect(0,0,100,100);
    brush.setStyle(Qt::SolidPattern);
    setZValue(-1);
    setHasChessPiece(false);
    setChessPieceColor("NONE");
    currentPiece = NULL;
}

ChessBox::~ChessBox()
{
    delete this;
}

void ChessBox::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
      //  qDebug() << getChessPieceColor();
        //Deselecting counter part of ChessPiece
        if(currentPiece == game->pieceToMove && currentPiece){

            currentPiece->mousePressEvent(event);
            return;
        }

        //if selected
        if(game->pieceToMove){
            //if same team
            if(this->getChessPieceColor() == game->pieceToMove->getSide())
                return;
            //removing the eaten piece
            QList <ChessBox *> movLoc = game->pieceToMove->moveLocation();
            //TO make sure the selected box is in move zone
            int check = 0;
            for(size_t i = 0, n = movLoc.size(); i < n;i++) {
                if(movLoc[i] == this) {
                    check++;
                }
            }
            // if not prsent return
            if(check == 0)
                return;
            //change the color back to normal
             game->pieceToMove->decolor();
             //make the first move false applicable for pawn only
             game->pieceToMove->firstMove = false;
             //this is to eat or consume the enemy present inn the movable region
            if(this->getHasChessPiece()){
                this->currentPiece->setIsPlaced(false);
                this->currentPiece->setCurrentBox(NULL);
                game->placeInDeadPlace(this->currentPiece);

            }

            int oldCol = game->pieceToMove->getCurrentBox()->colLoc;
            int oldRow = game->pieceToMove->getCurrentBox()->rowLoc;
            //changing the new stat and resetting the previous left region
            game->pieceToMove->getCurrentBox()->setHasChessPiece(false);
            game->pieceToMove->getCurrentBox()->currentPiece = NULL;
            game->pieceToMove->getCurrentBox()->resetOriginalColor();
            placePiece(game->pieceToMove);

            int newCol = colLoc;
            int newRow = rowLoc;



            if (clientManager) {
                clientManager->sendMove(game->match_id,user->getUsername(),7-oldRow,7-oldCol,7-newRow,7-newCol);
            }

            game->pieceToMove = NULL;
            //changing turn
            game->changeTurn();
            checkForCheck();

            // ChessBox *oldBox = game->collection[1][1];
            // ChessBox *newBox = game->collection[2][1];

            // oldBox->updateOpponentMove(oldBox,newBox);
            //updateOpponentMove(1,1,2,1);

        }
        //Selecting couterpart of the chessPiece
        else if(this->getHasChessPiece())
        {
            this->currentPiece->mousePressEvent(event);
        }
}

void ChessBox::updateOpponentMove(int old_col,int old_row, int new_col,int new_row){
    //qDebug() << "Test xem như nào:" << game->collection[1][1]->currentPiece->getSide();

    ChessBox *oldBox = game->collection[old_row][old_col];
    ChessBox *newBox = game->collection[new_row][new_col];

    //qDebug() << "OldBox Address:" << oldBox;
    //qDebug() << "NewBox Address:" << newBox;

    if (!oldBox || !newBox) {
        qDebug() << "Error: One of the boxes is NULL!";
        return;
    }

    //qDebug() << "OldBox Position:" << oldBox->getPosition();
    //qDebug() << "NewBox Position:" << newBox->getPosition();

    ChessPiece *piece = oldBox->currentPiece;

    //qDebug() << "Piece Address:" << piece;

    if (!piece) {
        qDebug() << "Error: No piece in oldBox!";
        return;
    }

    //qDebug() << "Piece Side:" << piece->getSide();

    // Update oldBox
    //qDebug() << "Setting oldBox hasChessPiece to false...";
    oldBox->setHasChessPiece(false);
    oldBox->currentPiece = NULL;

    //qDebug() << "oldBox hasChessPiece after update:" << oldBox->getHasChessPiece();
    //qDebug() << "oldBox currentPiece after update:" << oldBox->currentPiece;

    // Update newBox
    //qDebug() << "Setting newBox hasChessPiece to true...";
    //newBox->setHasChessPiece(true, piece);
    //qDebug() << "newBox hasChessPiece after update:" << newBox->getHasChessPiece();

    //newBox->currentPiece = piece;
    //qDebug() << "newBox currentPiece after update:" << newBox->currentPiece;

    // Update piece
    //qDebug() << "Updating piece's currentBox...";
    //piece->setCurrentBox(newBox);
    //qDebug() << "Piece's currentBox after update:" << piece->getCurrentBox()->getPosition();
    if(newBox->getHasChessPiece()){
        newBox->currentPiece->setIsPlaced(false);
        newBox->currentPiece->setCurrentBox(NULL);
        game->placeInDeadPlace(this->currentPiece);

    }




    newBox->placePiece(piece);
    game->changeTurn();
    checkForCheck();
}




void ChessBox::setColor(QColor color)
{
    brush.setColor(color);
    setBrush(color);
}



void ChessBox::placePiece(ChessPiece *piece)
{
    piece->setPos(x()+50- piece->pixmap().width()/2  ,y()+50-piece->pixmap().width()/2);
    piece->setCurrentBox(this);
    setHasChessPiece(true,piece);
    currentPiece = piece;


}

void ChessBox::resetOriginalColor()
{
    setColor(originalColor);
}



void ChessBox::setOriginalColor(QColor value)
{
    originalColor = value;
    setColor(originalColor);
}

bool ChessBox::getHasChessPiece()
{
    return hasChessPiece;
}

void ChessBox::setHasChessPiece(bool value, ChessPiece *piece)
{
    hasChessPiece = value;
    if(value)
        setChessPieceColor(piece->getSide());
    else
        setChessPieceColor("NONE");
}

void ChessBox::checkForCheck()
{
    int c = 0;
    QList <ChessPiece *> pList = game->alivePiece;
    for(size_t i = 0,n=pList.size(); i < n; i++ ) {

        King * p = dynamic_cast<King *> (pList[i]);
        if(p){
            continue;
        }
        pList[i]->moves();
        pList[i]->decolor();
        QList <ChessBox *> bList = pList[i]->moveLocation();
        for(size_t j = 0,n = bList.size(); j < n; j ++) {
            King * p = dynamic_cast<King *> (bList[j]->currentPiece);
            if(p) {
                if(p->getSide() == pList[i]->getSide())
                    continue;
                bList[j]->setColor(Qt::blue);
                pList[i]->getCurrentBox()->setColor(Qt::darkRed);
                if(!game->check->isVisible())
                    game->check->setVisible(true);
                else{
                    bList[j]->resetOriginalColor();
                    pList[i]->getCurrentBox()->resetOriginalColor();
                    game->gameOver();
                }
                c++;

            }
        }


    }
    if(!c){
        game->check->setVisible(false);
        for(size_t i = 0,n=pList.size(); i < n; i++ )
            pList[i]->getCurrentBox()->resetOriginalColor();
    }
}

QString ChessBox::getChessPieceColor()
{
    return chessPieceColor;
}

QString ChessBox::getPosition()
{
    return QString::number(this->rowLoc) + "-" + QString::number(this->colLoc);
}


void ChessBox::setChessPieceColor(QString value)
{
    chessPieceColor = value;
}

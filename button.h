#ifndef BUTTON_H
#define BUTTON_H
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>


class Button:public QObject, public QGraphicsRectItem
{
     Q_OBJECT
public:

    Button(QString name, QGraphicsItem * parent = NULL);

    //public methods

    //events
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void setSize(int width, int height);
    void setText(const QString &newText);
    void setFont();
    void alignText(int x, int y);
signals:
    void clicked();
private:
    QGraphicsTextItem *text;
};
#endif // BUTTON_H

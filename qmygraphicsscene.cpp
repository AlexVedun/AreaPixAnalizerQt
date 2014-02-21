#include "qmygraphicsscene.h"

QMyGraphicsScene::QMyGraphicsScene(QObject *parent) :
    QGraphicsScene(parent)
{
}

void QMyGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *pe)
{
    TopLeft = pe->scenePos();
    emit MouseButPress();
}

void QMyGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *pe)
{
    BottomRight = pe->scenePos();
    emit MouseButRelease();
}

void QMyGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *pe)
{
    if (pe->buttons()==Qt::LeftButton)
    {
        BottomRight = pe->scenePos();
        emit MouseButMove();
    }
}

QPointF QMyGraphicsScene::getTopLeft()
{
    return TopLeft;
}

QPointF QMyGraphicsScene::getBottomRight()
{
    return BottomRight;
}

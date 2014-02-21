#ifndef QMYGRAPHICSSCENE_H
#define QMYGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

class QMyGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit QMyGraphicsScene(QObject *parent = 0);
    QPointF getTopLeft ();
    QPointF getBottomRight ();

private:
    QPointF TopLeft, BottomRight;

protected:
    void mousePressEvent (QGraphicsSceneMouseEvent *pe);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *pe);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *pe);

signals:
    void MouseButPress ();
    void MouseButRelease ();
    void MouseButMove();

public slots:

};

#endif // QMYGRAPHICSSCENE_H

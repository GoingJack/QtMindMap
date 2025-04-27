#ifndef INFINITECANVAS_H
#define INFINITECANVAS_H

#include <QGraphicsView>

class InfiniteCanvas : public QGraphicsView
{
public:
    InfiniteCanvas(QGraphicsScene *scene, QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;


private:
    qreal m_scale_factor; // Tracks current scale factor
    qreal m_max_scale;    // Maximum allowed scale factor (4x)
    qreal m_min_scale;    // Minimum allowed scale factor
};

#endif // INFINITECANVAS_H

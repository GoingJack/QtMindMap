#ifndef INFINITECANVAS_H
#define INFINITECANVAS_H

#include <QGraphicsView>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

class InfiniteCanvas : public QGraphicsView
{
public:
    InfiniteCanvas(QGraphicsScene *scene, QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
    
    // Drag and drop event handlers
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    // Helper methods for drag and drop
    void handleImageDrop(const QMimeData *mime_data, const QPointF &pos);
    void handleTextDrop(const QMimeData *mime_data, const QPointF &pos);

    qreal m_scale_factor; // Tracks current scale factor
    qreal m_max_scale;    // Maximum allowed scale factor (4x)
    qreal m_min_scale;    // Minimum allowed scale factor
};

#endif // INFINITECANVAS_H

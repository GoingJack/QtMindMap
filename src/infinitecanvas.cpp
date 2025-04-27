#include "infinitecanvas.h"

#include <QWheelEvent>

InfiniteCanvas::InfiniteCanvas(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent),
      m_scale_factor(1.0),
      m_max_scale(4.0),
      m_min_scale(0.1) {
  setDragMode(QGraphicsView::ScrollHandDrag);
  // Only enable antialiasing for shapes, not for grid points
  setRenderHints(QPainter::Antialiasing);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  setResizeAnchor(QGraphicsView::AnchorUnderMouse);
  setBackgroundBrush(Qt::white);
  setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
}

void InfiniteCanvas::wheelEvent(QWheelEvent *event) {
  qreal factor = 1.03;

  if (event->angleDelta().y() > 0) {
    // Zooming in - check if we're at max zoom
    if (m_scale_factor * factor <= m_max_scale) {
      scale(factor, factor);
      m_scale_factor *= factor;
    }
  } else {
    // Zooming out
    qreal zoom_out_factor = 1.0 / factor;
    if (m_scale_factor * zoom_out_factor >= m_min_scale) {
      scale(zoom_out_factor, zoom_out_factor);
      m_scale_factor *= zoom_out_factor;
    }
  }
}

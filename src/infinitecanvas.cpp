#include "infinitecanvas.h"

#include <QWheelEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QImageReader>
#include <QUrl>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFont>

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
  
  // Enable dropping
  setAcceptDrops(true);
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

void InfiniteCanvas::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept if the drag event contains image data or text
    if (event->mimeData()->hasImage() || 
        event->mimeData()->hasUrls() || 
        event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void InfiniteCanvas::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void InfiniteCanvas::dropEvent(QDropEvent *event)
{
    // Convert the drop position to scene coordinates
    QPointF scene_pos = mapToScene(event->pos());
    
    const QMimeData *mime_data = event->mimeData();
    
    // Handle image drops
    if (mime_data->hasImage() || mime_data->hasUrls()) {
        handleImageDrop(mime_data, scene_pos);
        event->acceptProposedAction();
    }
    // Handle text drops
    else if (mime_data->hasText()) {
        handleTextDrop(mime_data, scene_pos);
        event->acceptProposedAction();
    }
}

void InfiniteCanvas::handleImageDrop(const QMimeData *mime_data, const QPointF &pos)
{
    QImage image;
    
    // Try to get image directly from mime data
    if (mime_data->hasImage()) {
        image = qvariant_cast<QImage>(mime_data->imageData());
    }
    // Try to load image from URLs
    else if (mime_data->hasUrls()) {
        QList<QUrl> urls = mime_data->urls();
        for (const QUrl &url : urls) {
            // Only handle local files
            if (url.isLocalFile()) {
                QString file_path = url.toLocalFile();
                QImageReader reader(file_path);
                if (reader.canRead()) {
                    image = reader.read();
                    break;
                }
            }
        }
    }
    
    if (!image.isNull()) {
        // Create a pixmap item from the image
        QGraphicsPixmapItem *pixmap_item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
        
        // Position the image at the drop position
        pixmap_item->setPos(pos);
        
        // Add to scene
        scene()->addItem(pixmap_item);
        
        // Make the item selectable and movable
        pixmap_item->setFlag(QGraphicsItem::ItemIsSelectable, true);
        pixmap_item->setFlag(QGraphicsItem::ItemIsMovable, true);
    }
}

void InfiniteCanvas::handleTextDrop(const QMimeData *mime_data, const QPointF &pos)
{
    if (mime_data->hasText()) {
        QString text = mime_data->text();
        
        if (!text.isEmpty()) {
            // Create a text item
            QGraphicsTextItem *text_item = new QGraphicsTextItem(text);
            
            // Set font and other properties
            QFont font("Arial", 12);
            text_item->setFont(font);
            text_item->setDefaultTextColor(Qt::black);
            
            // Position the text at the drop position
            text_item->setPos(pos);
            
            // Add to scene
            scene()->addItem(text_item);
            
            // Make the item selectable and movable
            text_item->setFlag(QGraphicsItem::ItemIsSelectable, true);
            text_item->setFlag(QGraphicsItem::ItemIsMovable, true);
        }
    }
}

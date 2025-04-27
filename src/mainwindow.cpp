#include "mainwindow.h"
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QScreen>
#include <QApplication>
#include <QWheelEvent>
#include <QPainter>
#include <QtMath>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>

class InfiniteCanvas : public QGraphicsView
{
public:
    InfiniteCanvas(QGraphicsScene *scene, QWidget *parent = nullptr)
        : QGraphicsView(scene, parent), m_scale_factor(1.0), m_max_scale(4.0), m_min_scale(0.1)
    {
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

protected:
    void wheelEvent(QWheelEvent *event) override
    {
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

private:
    qreal m_scale_factor; // Tracks current scale factor
    qreal m_max_scale;    // Maximum allowed scale factor (4x)
    qreal m_min_scale;    // Minimum allowed scale factor
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle(tr("QtMindMap"));
  
    QWidget *central_widget = new QWidget(this);
    setCentralWidget(central_widget);
  
    QVBoxLayout *layout = new QVBoxLayout(central_widget);
    layout->setContentsMargins(0, 0, 0, 0);
    
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-5000, -5000, 10000, 10000);
    
    // Add a red rectangle to the scene
    QGraphicsRectItem *rect_item = new QGraphicsRectItem(-100, -50, 200, 100);
    rect_item->setBrush(QBrush(Qt::red));
    rect_item->setPen(QPen(Qt::black, 2));
    m_scene->addItem(rect_item);
    
    // Add a yellow circle to the scene
    QGraphicsEllipseItem *circle_item = new QGraphicsEllipseItem(150, -100, 150, 150);
    circle_item->setBrush(QBrush(Qt::yellow));
    circle_item->setPen(QPen(Qt::black, 2));
    m_scene->addItem(circle_item);
    
    m_graphics_view = new InfiniteCanvas(m_scene, this);
    
    layout->addWidget(m_graphics_view);
    
    resize(1200, 800);
    
    QScreen *screen = QApplication::primaryScreen();
    QRect screen_geometry = screen->availableGeometry();
    int x = (screen_geometry.width() - width()) / 2;
    int y = (screen_geometry.height() - height()) / 2;
    move(x, y);
}

MainWindow::~MainWindow() {
}

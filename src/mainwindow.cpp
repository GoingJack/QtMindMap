#include "mainwindow.h"

#include <QApplication>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QScreen>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QtMath>

#include "infinitecanvas.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
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
  QGraphicsEllipseItem *circle_item =
      new QGraphicsEllipseItem(150, -100, 150, 150);
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

MainWindow::~MainWindow() {}

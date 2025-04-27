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
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

#include "infinitecanvas.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle(tr("QtMindMap"));

  // Create menu bar
  setupMenus();

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

void MainWindow::setupMenus() {
  // Create File menu
  QMenu *file_menu = menuBar()->addMenu(tr("File"));
  
  // Add New action
  QAction *new_action = new QAction(tr("New"), this);
  file_menu->addAction(new_action);
  connect(new_action, &QAction::triggered, this, &MainWindow::newFile);
  
  // Add Open action
  QAction *open_action = new QAction(tr("Open"), this);
  file_menu->addAction(open_action);
  connect(open_action, &QAction::triggered, this, &MainWindow::openFile);
  
  // Add Save action
  QAction *save_action = new QAction(tr("Save"), this);
  file_menu->addAction(save_action);
  connect(save_action, &QAction::triggered, this, &MainWindow::saveFile);
  
  file_menu->addSeparator();
  
  // Add Exit action
  QAction *exit_action = new QAction(tr("Exit"), this);
  file_menu->addAction(exit_action);
  connect(exit_action, &QAction::triggered, this, &MainWindow::close);
  
  // Create Edit menu
  QMenu *edit_menu = menuBar()->addMenu(tr("Edit"));
  
  // Add Copy action
  QAction *copy_action = new QAction(tr("Copy"), this);
  edit_menu->addAction(copy_action);
  
  // Add Paste action
  QAction *paste_action = new QAction(tr("Paste"), this);
  edit_menu->addAction(paste_action);
  
  // Add Delete action
  QAction *delete_action = new QAction(tr("Delete"), this);
  edit_menu->addAction(delete_action);
  
  // Create Help menu
  QMenu *help_menu = menuBar()->addMenu(tr("Help"));
  
  // Add About action
  QAction *about_action = new QAction(tr("About"), this);
  help_menu->addAction(about_action);
  connect(about_action, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::newFile() {
  // Clear the scene
  m_scene->clear();
}

void MainWindow::openFile() {
  // Show open file dialog
  QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Mind Map Files (*.mm);;All Files (*)"));
  if (!file_name.isEmpty()) {
    // Handle file opening
  }
}

void MainWindow::saveFile() {
  // Show save file dialog
  QString file_name = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Mind Map Files (*.mm);;All Files (*)"));
  if (!file_name.isEmpty()) {
    // Handle file saving
  }
}

void MainWindow::showAbout() {
  QMessageBox::about(this, tr("About QtMindMap"),
                    tr("QtMindMap is a simple mind mapping application.\n"
                       "Supports drag and drop of images and text."));
}

MainWindow::~MainWindow() {}

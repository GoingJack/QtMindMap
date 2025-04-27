#include "mainwindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
  setWindowTitle(tr("QtMindMap"));
  
  QWidget *central_widget = new QWidget(this);
  setCentralWidget(central_widget);
  
  QVBoxLayout *layout = new QVBoxLayout(central_widget);
  
  resize(800, 600);
}

MainWindow::~MainWindow() {
}

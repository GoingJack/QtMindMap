#include "pch.h"

#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QSharedMemory sharedMemory("QtMindMap");
  if (sharedMemory.attach()) {
    return 0;
  }
  sharedMemory.create(1);
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}

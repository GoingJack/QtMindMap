#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>

class InfiniteCanvas;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void showAbout();

private:
    void setupMenus();
    void saveToFile(const QString &file_name);
    void loadFromFile(const QString &file_name);
    
    InfiniteCanvas *m_graphics_view;
    QGraphicsScene *m_scene;
};

#endif // MAINWINDOW_H

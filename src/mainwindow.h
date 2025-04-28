#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QString>
#include <QAction>

class InfiniteCanvas;
class ShortcutItem;
class UrlItem;

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
    void resetZoom();
    void toggleAlwaysOnTop(bool checked);

private:
    void setupMenus();
    void saveToFile(const QString &file_name);
    void loadFromFile(const QString &file_name);
    
    // Recent file handling
    void saveRecentFilePath(const QString &file_path);
    QString loadRecentFilePath();
    QString getSettingsFilePath();
    void tryLoadRecentFile();
    
    InfiniteCanvas *m_graphics_view;
    QGraphicsScene *m_scene;
    QString m_current_file;
    QAction *m_always_on_top_action;
};

#endif // MAINWINDOW_H

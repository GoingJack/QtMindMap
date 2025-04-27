#ifndef INFINITECANVAS_H
#define INFINITECANVAS_H

#include <QGraphicsView>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QContextMenuEvent>
#include <QGraphicsPixmapItem>

// Custom shortcut item class
class ShortcutItem : public QGraphicsPixmapItem
{
public:
    ShortcutItem(const QPixmap &pixmap, const QString &target_path, QGraphicsItem *parent = nullptr);
    
    // Get the target path of the shortcut
    QString getTargetPath() const { return m_target_path; }
    
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    
private:
    QString m_target_path;
};

// Custom directory item class
class DirectoryItem : public QGraphicsPixmapItem
{
public:
    DirectoryItem(const QPixmap &pixmap, const QString &dir_path, QGraphicsItem *parent = nullptr);
    
    // Get the directory path
    QString getDirPath() const { return m_dir_path; }
    
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    
private:
    QString m_dir_path;
};

class InfiniteCanvas : public QGraphicsView
{
public:
    InfiniteCanvas(QGraphicsScene *scene, QWidget *parent = nullptr);
    
    // Scale factor getter and setter
    qreal getScaleFactor() const { return m_scale_factor; }
    void setScaleFactor(qreal factor) { m_scale_factor = factor; }

protected:
    void wheelEvent(QWheelEvent *event) override;
    
    // Drag and drop event handlers
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    // Context menu event handler
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    // Helper methods for drag and drop
    void handleImageDrop(const QMimeData *mime_data, const QPointF &pos);
    void handleTextDrop(const QMimeData *mime_data, const QPointF &pos);
    void handleShortcutDrop(const QUrl &url, const QPointF &pos);
    void handleDirectoryDrop(const QUrl &url, const QPointF &pos);
    
    // Helper method to delete selected items
    void deleteSelectedItems();
    
    // Helper methods for file/directory handling
    QPixmap getFileIcon(const QString &file_path);
    QPixmap getDirectoryIcon(const QString &dir_path);
    bool isDirectory(const QString &path);
    void openDirectory(const QString &dir_path);
    
    // Helper method to resolve shortcut target
    QString resolveShortcutTarget(const QString &shortcut_path);

    qreal m_scale_factor; // Tracks current scale factor
    qreal m_max_scale;    // Maximum allowed scale factor (4x)
    qreal m_min_scale;    // Minimum allowed scale factor
};

#endif // INFINITECANVAS_H

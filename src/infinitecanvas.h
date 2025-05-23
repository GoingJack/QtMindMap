#ifndef INFINITECANVAS_H
#define INFINITECANVAS_H

#include "pch.h"

// Forward declarations
class EditableTextItem;
class ConnectionLine;

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

// Custom URL item class
class UrlItem : public QGraphicsItemGroup
{
public:
    UrlItem(const QPixmap &pixmap, const QUrl &url, QGraphicsItem *parent = nullptr);
    
    // Get the URL
    QUrl getUrl() const { return m_url; }
    
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    
private:
    QUrl m_url;
    QGraphicsPixmapItem *m_icon_item;
    QGraphicsSimpleTextItem *m_label_item;
    
    // Helper method to get website domain from URL
    QString getDomainFromUrl(const QUrl &url);
};

// Custom directory item class
class DirectoryItem : public QGraphicsItemGroup
{
public:
    DirectoryItem(const QPixmap &pixmap, const QString &dir_path, QGraphicsItem *parent = nullptr);
    
    // Get the directory path
    QString getDirPath() const { return m_dir_path; }
    
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    
private:
    QString m_dir_path;
    QGraphicsPixmapItem *m_icon_item;
    QGraphicsSimpleTextItem *m_label_item;
    
    // Helper method to get directory name from path
    QString getDirName(const QString &path);
};

// Custom media item class for audio/video files
class MediaItem : public QGraphicsItemGroup
{
public:
    MediaItem(const QPixmap &pixmap, const QString &media_path, QGraphicsItem *parent = nullptr);
    
    // Get the media file path
    QString getMediaPath() const { return m_media_path; }
    
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    
private:
    QString m_media_path;
    QGraphicsPixmapItem *m_icon_item;
    QGraphicsSimpleTextItem *m_label_item;
    
    // Helper method to get file name from path
    QString getFileName(const QString &path);
};

// Connection line between nodes
class ConnectionLine : public QGraphicsPathItem
{
public:
    ConnectionLine(EditableTextItem *from_item, EditableTextItem *to_item, QGraphicsItem *parent = nullptr);
    
    // Update line position based on connected items
    void updatePosition();
    
    // Get source and target items
    EditableTextItem* sourceItem() const { return m_source_item; }
    EditableTextItem* targetItem() const { return m_target_item; }
    
    // Set color based on level or index
    void setColorByIndex(int index);
    
private:
    EditableTextItem *m_source_item;
    EditableTextItem *m_target_item;
    int m_level;
    qreal m_curve_factor; // Controls the curve amount
};

// Custom text item with double-click editing
class EditableTextItem : public QGraphicsTextItem
{
public:
    EditableTextItem(const QString &text, QGraphicsItem *parent = nullptr);
    ~EditableTextItem();
    
    // Add or remove a child node
    void addChildNode(EditableTextItem *child_node);
    void removeChildNode(EditableTextItem *child_node);
    
    // Get parent and child nodes
    EditableTextItem* parentNode() const { return m_parent_node; }
    QList<EditableTextItem*> childNodes() const { return m_child_nodes; }
    
    // Set parent node
    void setParentNode(EditableTextItem *parent);
    
    // Update all connections
    void updateConnections();
    
    // Organize layout of children
    void organizeChildrenLayout();
    
    // Override paint to draw border
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    
    // Override boundingRect to include border
    QRectF boundingRect() const override;
    
    // Get depth level in tree (root=0)
    int getDepthLevel() const;
    
    // Get total height requirement for this node and all descendants
    qreal getTotalHeightRequirement() const;
    
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    
private:
    QPointer<EditableTextItem> m_parent_node;
    QList<EditableTextItem*> m_child_nodes;
    QList<ConnectionLine*> m_connections;
    qreal m_padding; // Padding around text for border
    
    // Position children symmetrically
    void positionChildrenSymmetrically();
};

class InfiniteCanvas : public QGraphicsView
{
    Q_OBJECT
public:
    InfiniteCanvas(QGraphicsScene *scene, QWidget *parent = nullptr);
    
    // Scale factor getter and setter
    qreal getScaleFactor() const { return m_scale_factor; }
    void setScaleFactor(qreal factor) { m_scale_factor = factor; }
    
    // Reset zoom to 100%
    void resetZoom();
    
    // Website icon getter
    QPixmap getWebsiteIcon(const QUrl &url);
    
    // Media file icon getter
    QPixmap getMediaIcon(const QString &media_path);
    
    // Paste from clipboard functionality
    void pasteFromClipboard();

    // Copy selected items to clipboard
    void copyToClipboard();

    // For MainWindow to access current zoom level
    qreal currentZoomFactor() const { return m_scale_factor; }
    
    // Create a new text node at the specified position
    EditableTextItem* createTextNode(const QPointF &position, const QString &text = "New Node");
    
    // Organize the entire mind map layout from the selected node
    void organizeLayoutFromNode(EditableTextItem* node);

protected:
    void wheelEvent(QWheelEvent *event) override;
    
    // Drag and drop event handlers
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    // Context menu event handler
    void contextMenuEvent(QContextMenuEvent *event) override;
    
    // Key press handler for shortcuts
    void keyPressEvent(QKeyEvent *event) override;

private:
    // Helper methods for drag and drop
    void handleImageDrop(const QMimeData *mime_data, const QPointF &pos);
    void handleTextDrop(const QMimeData *mime_data, const QPointF &pos);
    void handleUrlDrop(const QString &url_str, const QPointF &pos);
    void handleShortcutDrop(const QUrl &url, const QPointF &pos);
    void handleDirectoryDrop(const QUrl &url, const QPointF &pos);
    void handleMediaDrop(const QUrl &url, const QPointF &pos);
    
    // Universal handler for mime data (used by both drag-drop and paste)
    bool processItemFromMimeData(const QMimeData *mime_data, const QPointF &pos);
    
    // Helper method to delete selected items
    void deleteSelectedItems();
    
    // Helper method to copy selected items to clipboard
    void copySelectedItemsToClipboard();
    
    // Helper methods for file/directory/url handling
    QPixmap getFileIcon(const QString &file_path);
    QPixmap getDirectoryIcon(const QString &dir_path);
    bool isDirectory(const QString &path);
    bool isUrl(const QString &text);
    bool isMediaFile(const QString &file_path);
    void openDirectory(const QString &dir_path);
    
    // Helper method to resolve shortcut target
    QString resolveShortcutTarget(const QString &shortcut_path);

    qreal m_scale_factor; // Tracks current scale factor
    qreal m_max_scale;    // Maximum allowed scale factor (4x)
    qreal m_min_scale;    // Minimum allowed scale factor
};

#endif // INFINITECANVAS_H

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
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QScrollBar>
#include <QProcess>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QGraphicsSceneMouseEvent>

// Windows specific includes for shortcut handling
#ifdef Q_OS_WIN
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#include <shobjidl.h>
#include <objidl.h>
#endif

// ShortcutItem implementation
ShortcutItem::ShortcutItem(const QPixmap &pixmap, const QString &target_path, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent), m_target_path(target_path) {
    // Enable item flags
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    
    // Store the target path as item data
    setData(0, target_path);
    setData(1, "shortcut"); // Mark as shortcut item
}

void ShortcutItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    // Launch the associated application
    if (!m_target_path.isEmpty()) {
        QProcess::startDetached(m_target_path, QStringList());
    }
    
    // Call the base implementation
    QGraphicsPixmapItem::mouseDoubleClickEvent(event);
}

// InfiniteCanvas implementation
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
    // Accept if the drag event contains image data, urls, or text
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
    
    // Handle URL drops (including shortcut files)
    if (mime_data->hasUrls()) {
        QList<QUrl> urls = mime_data->urls();
        bool handled = false;
        
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                QString file_path = url.toLocalFile();
                QFileInfo file_info(file_path);
                
                // Check if it's a Windows shortcut file
                if (file_info.suffix().toLower() == "lnk") {
                    handleShortcutDrop(url, scene_pos);
                    handled = true;
                    break;
                }
            }
        }
        
        // If not handled as a shortcut, try handling as an image
        if (!handled && (mime_data->hasImage() || mime_data->hasUrls())) {
            handleImageDrop(mime_data, scene_pos);
            handled = true;
        }
        
        if (handled) {
            event->acceptProposedAction();
            return;
        }
    }
    
    // Handle text drops
    if (mime_data->hasText()) {
        handleTextDrop(mime_data, scene_pos);
        event->acceptProposedAction();
    }
}

void InfiniteCanvas::handleImageDrop(const QMimeData *mime_data, const QPointF &pos)
{
    QImage image;
    QString file_path;
    
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
                file_path = url.toLocalFile();
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
        
        // Store the file path for serialization
        if (!file_path.isEmpty()) {
            pixmap_item->setData(0, file_path);
        }
        
        // Add to scene
        scene()->addItem(pixmap_item);
        
        // Make the item selectable and movable
        pixmap_item->setFlag(QGraphicsItem::ItemIsSelectable, true);
        pixmap_item->setFlag(QGraphicsItem::ItemIsMovable, true);
    }
}

void InfiniteCanvas::handleShortcutDrop(const QUrl &url, const QPointF &pos)
{
    QString shortcut_path = url.toLocalFile();
    QString target_path = resolveShortcutTarget(shortcut_path);
    
    if (!target_path.isEmpty()) {
        // Get icon for the shortcut
        QPixmap icon = getFileIcon(target_path);
        
        // Create the shortcut item
        ShortcutItem *shortcut_item = new ShortcutItem(icon, target_path);
        
        // Position at drop location
        shortcut_item->setPos(pos);
        
        // Add to scene
        scene()->addItem(shortcut_item);
        
        // Set tooltip to show target path
        shortcut_item->setToolTip(target_path);
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

// Context menu event handler implementation
void InfiniteCanvas::contextMenuEvent(QContextMenuEvent *event)
{
    // Get the position of the event in scene coordinates
    QPointF scene_pos = mapToScene(event->pos());
    
    // Create a menu
    QMenu context_menu(this);
    
    // Check if any items are selected
    if (!scene()->selectedItems().isEmpty()) {
        // Add delete action to the menu
        QAction *delete_action = context_menu.addAction("Delete");
        connect(delete_action, &QAction::triggered, this, &InfiniteCanvas::deleteSelectedItems);
    }
    
    // Only show the menu if it's not empty
    if (!context_menu.actions().isEmpty()) {
        context_menu.exec(event->globalPos());
    } else {
        // If no menu items, pass the event to the parent class
        QGraphicsView::contextMenuEvent(event);
    }
}

// Method to delete selected items
void InfiniteCanvas::deleteSelectedItems()
{
    // Get the list of selected items
    QList<QGraphicsItem*> selected_items = scene()->selectedItems();
    
    // Delete each selected item
    for (QGraphicsItem *item : selected_items) {
        scene()->removeItem(item);
        delete item;
    }
}

// Get file icon
QPixmap InfiniteCanvas::getFileIcon(const QString &file_path)
{
    QFileIconProvider icon_provider;
    QFileInfo file_info(file_path);
    QIcon icon = icon_provider.icon(file_info);
    
    // Get a reasonable size for the icon
    QPixmap pixmap = icon.pixmap(64, 64);
    
    // If we got an empty pixmap, use a default
    if (pixmap.isNull()) {
        pixmap = QPixmap(64, 64);
        pixmap.fill(Qt::transparent);
    }
    
    return pixmap;
}

// Resolve shortcut target path
QString InfiniteCanvas::resolveShortcutTarget(const QString &shortcut_path)
{
    QString target;
    
#ifdef Q_OS_WIN
    // Initialize COM
    HRESULT hr = CoInitialize(NULL);
    if (SUCCEEDED(hr)) {
        IShellLinkW* psl;
        
        // Get a pointer to the IShellLink interface (explicitly use Unicode version)
        hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                             IID_IShellLinkW, (void**)&psl);
        
        if (SUCCEEDED(hr)) {
            IPersistFile* ppf;
            
            // Get a pointer to the IPersistFile interface
            hr = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
            
            if (SUCCEEDED(hr)) {
                // Load the shortcut
                hr = ppf->Load((LPCWSTR)shortcut_path.utf16(), STGM_READ);
                
                if (SUCCEEDED(hr)) {
                    // Get the target path
                    WCHAR szTargetPath[MAX_PATH];
                    hr = psl->GetPath(szTargetPath, MAX_PATH, NULL, 0);
                    
                    if (SUCCEEDED(hr)) {
                        target = QString::fromWCharArray(szTargetPath);
                    }
                }
                
                // Release the IPersistFile interface
                ppf->Release();
            }
            
            // Release the IShellLink interface
            psl->Release();
        }
        
        // Uninitialize COM
        CoUninitialize();
    }
#endif
    
    return target;
}

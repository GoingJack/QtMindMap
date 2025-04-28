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
#include <QDir>
#include <QDesktopServices>
#include <QRegularExpression>
#include <QPainter>

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

// DirectoryItem implementation
DirectoryItem::DirectoryItem(const QPixmap &pixmap, const QString &dir_path, QGraphicsItem *parent)
    : QGraphicsItemGroup(parent), m_dir_path(dir_path) {
    // Enable item flags
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    
    // Create the icon item
    m_icon_item = new QGraphicsPixmapItem(pixmap, this);
    addToGroup(m_icon_item);
    
    // Create the text label
    QString dir_name = getDirName(dir_path);
    m_label_item = new QGraphicsSimpleTextItem(dir_name, this);
    
    // Set font for label
    QFont label_font("Arial", 10);
    m_label_item->setFont(label_font);
    
    // Position label below icon
    int icon_width = pixmap.width();
    int icon_height = pixmap.height();
    int label_width = m_label_item->boundingRect().width();
    
    // Center the label below the icon
    m_label_item->setPos((icon_width - label_width) / 2, icon_height + 5);
    
    addToGroup(m_label_item);
    
    // Store the directory path as item data
    setData(0, dir_path);
    setData(1, "directory"); // Mark as directory item
}

void DirectoryItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    // Open the directory in file explorer/Finder
    if (!m_dir_path.isEmpty()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_dir_path));
    }
    
    // Call the base implementation
    QGraphicsItemGroup::mouseDoubleClickEvent(event);
}

// Helper method to get directory name from path
QString DirectoryItem::getDirName(const QString &path) {
    QFileInfo file_info(path);
    QString name = file_info.fileName();
    
    // If empty (might be a root directory), use the absolute path
    if (name.isEmpty()) {
        name = file_info.absoluteFilePath();
        
        // For Windows drives, clean up the name
        if (name.endsWith(":/") || name.endsWith(":\\")) {
            name = name.left(name.length() - 2);
        }
    }
    
    // Limit length to avoid very long names
    const int MAX_NAME_LENGTH = 20;
    if (name.length() > MAX_NAME_LENGTH) {
        name = name.left(MAX_NAME_LENGTH - 3) + "...";
    }
    
    return name;
}

// UrlItem implementation
UrlItem::UrlItem(const QPixmap &pixmap, const QUrl &url, QGraphicsItem *parent)
    : QGraphicsItemGroup(parent), m_url(url) {
    // Enable item flags
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    
    // Create the icon item
    m_icon_item = new QGraphicsPixmapItem(pixmap, this);
    addToGroup(m_icon_item);
    
    // Create the text label
    QString domain = getDomainFromUrl(url);
    m_label_item = new QGraphicsSimpleTextItem(domain, this);
    
    // Set font for label
    QFont label_font("Arial", 10);
    m_label_item->setFont(label_font);
    
    // Position label below icon
    int icon_width = pixmap.width();
    int icon_height = pixmap.height();
    int label_width = m_label_item->boundingRect().width();
    
    // Center the label below the icon
    m_label_item->setPos((icon_width - label_width) / 2, icon_height + 5);
    
    addToGroup(m_label_item);
    
    // Store the URL as item data
    setData(0, url.toString());
    setData(1, "url"); // Mark as URL item
}

void UrlItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    // Open the URL in the default browser
    if (m_url.isValid()) {
        QDesktopServices::openUrl(m_url);
    }
    
    // Call the base implementation
    QGraphicsItemGroup::mouseDoubleClickEvent(event);
}

// Helper method to get domain from URL
QString UrlItem::getDomainFromUrl(const QUrl &url) {
    QString host = url.host();
    
    // Remove 'www.' prefix if present
    if (host.startsWith("www.")) {
        host = host.mid(4);
    }
    
    // Limit length to avoid very long names
    const int MAX_NAME_LENGTH = 20;
    if (host.length() > MAX_NAME_LENGTH) {
        host = host.left(MAX_NAME_LENGTH - 3) + "...";
    }
    
    return host;
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
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  setResizeAnchor(QGraphicsView::AnchorUnderMouse);
  setBackgroundBrush(Qt::white);
  setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
  
  // Enable dropping
  setAcceptDrops(true);
}

void InfiniteCanvas::wheelEvent(QWheelEvent *event) {
  // Only zoom if Ctrl key is pressed
  if (event->modifiers() & Qt::ControlModifier) {
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
  } else {
    // Pass the event to parent for normal scrolling
    QGraphicsView::wheelEvent(event);
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
    
    // Handle URL drops (including shortcuts and directories)
    if (mime_data->hasUrls()) {
        QList<QUrl> urls = mime_data->urls();
        bool handled = false;
        
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                QString file_path = url.toLocalFile();
                QFileInfo file_info(file_path);
                
                // Check if it's a directory
                if (isDirectory(file_path)) {
                    handleDirectoryDrop(url, scene_pos);
                    handled = true;
                    break;
                }
                // Check if it's a Windows shortcut file
                else if (file_info.suffix().toLower() == "lnk") {
                    handleShortcutDrop(url, scene_pos);
                    handled = true;
                    break;
                }
            }
        }
        
        // If not handled as a directory or shortcut, try handling as an image
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

void InfiniteCanvas::handleDirectoryDrop(const QUrl &url, const QPointF &pos)
{
    QString dir_path = url.toLocalFile();
    
    if (isDirectory(dir_path)) {
        // Get directory icon
        QPixmap icon = getDirectoryIcon(dir_path);
        
        // Create the directory item
        DirectoryItem *dir_item = new DirectoryItem(icon, dir_path);
        
        // Position at drop location
        dir_item->setPos(pos);
        
        // Add to scene
        scene()->addItem(dir_item);
        
        // Set tooltip to show complete directory path
        dir_item->setToolTip(dir_path);
    }
}

void InfiniteCanvas::handleTextDrop(const QMimeData *mime_data, const QPointF &pos)
{
    if (mime_data->hasText()) {
        QString text = mime_data->text();
        
        if (!text.isEmpty()) {
            // Check if the text is a URL
            if (isUrl(text)) {
                handleUrlDrop(text, pos);
            } else {
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
                
                // Enable text editing
                text_item->setTextInteractionFlags(Qt::TextEditorInteraction);
            }
        }
    }
}

// Handle URL drop
void InfiniteCanvas::handleUrlDrop(const QString &url_str, const QPointF &pos)
{
    // Normalize URL format
    QString normalized_url = url_str;
    
    // Add http:// prefix if missing
    if (!normalized_url.startsWith("http://") && !normalized_url.startsWith("https://") && !normalized_url.startsWith("ftp://")) {
        normalized_url = "http://" + normalized_url;
    }
    
    QUrl url(normalized_url);
    
    if (url.isValid()) {
        // Get icon for the website
        QPixmap icon = getWebsiteIcon(url);
        
        // Create the URL item
        UrlItem *url_item = new UrlItem(icon, url);
        
        // Position at drop location
        url_item->setPos(pos);
        
        // Add to scene
        scene()->addItem(url_item);
        
        // Set tooltip to show full URL
        url_item->setToolTip(url.toString());
    }
}

// Get website icon using network request to fetch favicon
QPixmap InfiniteCanvas::getWebsiteIcon(const QUrl &url)
{
    // Create a default icon with domain initial
    QPixmap fallback_icon(64, 64);
    fallback_icon.fill(Qt::lightGray);
    
    QPainter painter(&fallback_icon);
    painter.setPen(Qt::blue);
    painter.setFont(QFont("Arial", 32, QFont::Bold));
    
    QString letter;
    if (url.host().isEmpty()) {
        letter = "W";
    } else {
        letter = QString(url.host().at(0).toUpper());
    }
    painter.drawText(fallback_icon.rect(), Qt::AlignCenter, letter);
    
    return fallback_icon;
}

// Check if text is a URL
bool InfiniteCanvas::isUrl(const QString &text)
{
    // URLs with protocol
    QRegularExpression urlWithProtocolRegex(
        "^(https?|ftp)://[^\\s/$.?#].[^\\s]*$",
        QRegularExpression::CaseInsensitiveOption
    );
    
    // URLs without protocol (www...)
    QRegularExpression wwwRegex(
        "^www\\.[^\\s/$.?#].[^\\s]*$", 
        QRegularExpression::CaseInsensitiveOption
    );
    
    // Domain-only URLs (example.com, example.org, etc.)
    QRegularExpression domainRegex(
        "^[a-zA-Z0-9][-a-zA-Z0-9]*\\.[a-zA-Z0-9][-a-zA-Z0-9]*(\\.[a-zA-Z0-9][-a-zA-Z0-9]*)*$",
        QRegularExpression::CaseInsensitiveOption
    );
    
    bool matchesProtocol = urlWithProtocolRegex.match(text).hasMatch();
    bool matchesWww = wwwRegex.match(text).hasMatch();
    bool matchesDomain = domainRegex.match(text).hasMatch();
    
    return matchesProtocol || matchesWww || matchesDomain;
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

// Check if the path is a directory
bool InfiniteCanvas::isDirectory(const QString &path)
{
    QFileInfo file_info(path);
    return file_info.isDir();
}

// Open directory in system file explorer
void InfiniteCanvas::openDirectory(const QString &dir_path)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir_path));
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

// Get directory icon
QPixmap InfiniteCanvas::getDirectoryIcon(const QString &dir_path)
{
    QFileIconProvider icon_provider;
    QFileInfo dir_info(dir_path);
    QIcon icon = icon_provider.icon(QFileIconProvider::Folder);
    
    // For specific folder, use its actual icon
    if (dir_info.exists()) {
        icon = icon_provider.icon(dir_info);
    }
    
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

// Reset zoom to 100%
void InfiniteCanvas::resetZoom() {
    // Save center point
    QPointF center_point = mapToScene(viewport()->rect().center());
    
    // Reset transform
    resetTransform();
    
    // Set scale factor back to 1.0
    m_scale_factor = 1.0;
    
    // Restore center point
    centerOn(center_point);
}

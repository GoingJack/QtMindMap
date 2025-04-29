#include "pch.h"

#include "infinitecanvas.h"

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

// MediaItem implementation
MediaItem::MediaItem(const QPixmap &pixmap, const QString &media_path, QGraphicsItem *parent)
    : QGraphicsItemGroup(parent), m_media_path(media_path) {
    // Enable item flags
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    
    // Create the icon item
    m_icon_item = new QGraphicsPixmapItem(pixmap, this);
    addToGroup(m_icon_item);
    
    // Create the text label
    QString file_name = getFileName(media_path);
    m_label_item = new QGraphicsSimpleTextItem(file_name, this);
    
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
    
    // Store the media path as item data
    setData(0, media_path);
    setData(1, "media"); // Mark as media item
}

void MediaItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    // Open the media file with default application
    if (!m_media_path.isEmpty()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_media_path));
    }
    
    // Call the base implementation
    QGraphicsItemGroup::mouseDoubleClickEvent(event);
}

// Helper method to get file name from path
QString MediaItem::getFileName(const QString &path) {
    QFileInfo file_info(path);
    QString name = file_info.fileName();
    
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

// ConnectionLine implementation
ConnectionLine::ConnectionLine(EditableTextItem *from_item, EditableTextItem *to_item, QGraphicsItem *parent)
    : QGraphicsPathItem(parent), m_source_item(from_item), m_target_item(to_item), m_level(0), m_curve_factor(50.0)
{
    // Set pen for the line
    QPen line_pen(QColor(70, 130, 180), 2.0); // Steel blue, thicker line
    line_pen.setStyle(Qt::SolidLine);
    setPen(line_pen);
    
    // Update initial position
    updatePosition();
    
    // Make sure the line is below the text items (drawn first)
    setZValue(-1);
    
    // Not selectable or movable directly (moves with nodes)
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    
    // Set color based on level/index
    int level = to_item->getDepthLevel();
    setColorByIndex(level);
}

void ConnectionLine::updatePosition()
{
    if (!m_source_item || !m_target_item) {
        return;
    }
    
    // Get source and target item boundaries
    QRectF source_rect = m_source_item->boundingRect().translated(m_source_item->pos());
    QRectF target_rect = m_target_item->boundingRect().translated(m_target_item->pos());
    
    // Get center points
    QPointF source_center = source_rect.center();
    QPointF target_center = target_rect.center();
    
    // Determine which side of the source node to use based on relative positions
    QPointF source_edge;
    QPointF target_edge;
    
    // Calculate midpoints of all sides of the source rectangle
    QPointF source_right_mid(source_rect.right(), source_rect.center().y());
    QPointF source_left_mid(source_rect.left(), source_rect.center().y());
    QPointF source_top_mid(source_rect.center().x(), source_rect.top());
    QPointF source_bottom_mid(source_rect.center().x(), source_rect.bottom());
    
    // Calculate midpoints of all sides of the target rectangle
    QPointF target_right_mid(target_rect.right(), target_rect.center().y());
    QPointF target_left_mid(target_rect.left(), target_rect.center().y());
    QPointF target_top_mid(target_rect.center().x(), target_rect.top());
    QPointF target_bottom_mid(target_rect.center().x(), target_rect.bottom());
    
    // Determine the best exit point from source based on relative position of target
    if (target_center.x() >= source_center.x()) {
        // Target is to the right of source
        source_edge = source_right_mid;
    } else {
        // Target is to the left of source
        source_edge = source_left_mid;
    }
    
    // Determine the best entry point to target based on relative position of source
    if (source_center.x() <= target_center.x()) {
        // Source is to the left of target
        target_edge = target_left_mid;
    } else {
        // Source is to the right of target
        target_edge = target_right_mid;
    }
    
    // Create a curved path 
    QPainterPath path;
    path.moveTo(source_edge);
    
    // Calculate distance between nodes for curve control
    qreal distance = QLineF(source_edge, target_edge).length();
    
    // Control point 1 - move from source along x-axis
    QPointF control1 = source_edge + QPointF(distance * 0.4, 0);
    
    // Control point 2 - move from target along x-axis
    QPointF control2 = target_edge - QPointF(distance * 0.4, 0);
    
    // Create the cubic Bezier curve
    path.cubicTo(control1, control2, target_edge);
    
    // Set the path
    setPath(path);
}

void ConnectionLine::setColorByIndex(int index)
{
    // Nice brown color for all connections
    QColor line_color = QColor(139, 69, 19); // Saddle Brown
    
    // Create a pen with this color
    QPen line_pen = pen();
    line_pen.setColor(line_color);
    setPen(line_pen);
}

// Updated EditableTextItem implementation
EditableTextItem::EditableTextItem(const QString &text, QGraphicsItem *parent)
    : QGraphicsTextItem(text, parent), m_parent_node(nullptr), m_padding(5.0)
{
    // Set flags for selection and movement
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    
    // Not editable by default
    setTextInteractionFlags(Qt::NoTextInteraction);
    
    // Set data type for identification
    setData(1, "text_node");
    
    // Set document margins to accommodate padding
    document()->setDocumentMargin(m_padding);
}

QRectF EditableTextItem::boundingRect() const
{
    // Get the base bounding rect from QGraphicsTextItem
    QRectF base_rect = QGraphicsTextItem::boundingRect();
    
    // Add padding for the border
    return base_rect.adjusted(-m_padding, -m_padding, m_padding, m_padding);
}

void EditableTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Save painter state
    painter->save();
    
    // Get the rectangle to draw
    QRectF rect = boundingRect();
    
    // Set up the pen for the border
    QPen border_pen(QColor(100, 149, 237)); // Cornflower blue
    border_pen.setWidth(1);
    painter->setPen(border_pen);
    
    // Set up the brush for the background
    QBrush background_brush(QColor(240, 248, 255)); // AliceBlue - very light blue
    painter->setBrush(background_brush);
    
    // Draw rounded rectangle
    qreal corner_radius = 8.0;
    painter->drawRoundedRect(rect, corner_radius, corner_radius);
    
    // Restore painter state before drawing the text
    painter->restore();
    
    // Let the base class draw the text content
    QGraphicsTextItem::paint(painter, option, widget);
}

EditableTextItem::~EditableTextItem()
{
    // Remove connections to parent
    if (m_parent_node) {
        m_parent_node->removeChildNode(this);
    }
    
    // Remove all connections to children
    for (ConnectionLine *connection : m_connections) {
        if (connection->scene()) {
            connection->scene()->removeItem(connection);
        }
        delete connection;
    }
    
    // Clear lists
    m_connections.clear();
    m_child_nodes.clear();
}

void EditableTextItem::addChildNode(EditableTextItem *child_node)
{
    if (!child_node || m_child_nodes.contains(child_node)) {
        return;
    }
    
    // Add to our children list
    m_child_nodes.append(child_node);
    
    // Set us as the parent of the child
    child_node->setParentNode(this);
    
    // Create connection line
    if (scene()) {
        ConnectionLine *connection = new ConnectionLine(this, child_node);
        m_connections.append(connection);
        scene()->addItem(connection);
    }
}

void EditableTextItem::removeChildNode(EditableTextItem *child_node)
{
    if (!child_node || !m_child_nodes.contains(child_node)) {
        return;
    }
    
    // Find and remove the connection to this child
    for (int i = 0; i < m_connections.size(); ++i) {
        ConnectionLine *connection = m_connections[i];
        if (connection->targetItem() == child_node) {
            if (connection->scene()) {
                connection->scene()->removeItem(connection);
            }
            m_connections.removeAt(i);
            delete connection;
            break;
        }
    }
    
    // Remove from children list
    m_child_nodes.removeOne(child_node);
    
    // Set child's parent to null if it was us
    if (child_node->parentNode() == this) {
        child_node->setParentNode(nullptr);
    }
}

void EditableTextItem::setParentNode(EditableTextItem *parent)
{
    // Don't set as parent if it's already a child
    if (m_child_nodes.contains(parent)) {
        return;
    }
    
    m_parent_node = parent;
}

void EditableTextItem::updateConnections()
{
    // Update all connections
    for (ConnectionLine *connection : m_connections) {
        connection->updatePosition();
    }
    
    // If we have a parent, it also needs to update the connection to us
    if (m_parent_node) {
        m_parent_node->updateConnections();
    }
}

void EditableTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // Enable editing on double click
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setFocus();
    
    // Change cursor to text editing cursor (I-beam)
    setCursor(Qt::IBeamCursor);
    
    // Call parent method
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void EditableTextItem::focusOutEvent(QFocusEvent *event)
{
    // Disable editing when focus is lost
    setTextInteractionFlags(Qt::NoTextInteraction);
    
    // Restore to view's default cursor
    unsetCursor();
    
    // Clear text selection
    QTextCursor cursor = textCursor();
    cursor.clearSelection();
    setTextCursor(cursor);
    
    // Call parent method
    QGraphicsTextItem::focusOutEvent(event);
}

void EditableTextItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    // Let the scene handle the context menu
    event->ignore();
}

void EditableTextItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Let the base class handle the movement
    QGraphicsTextItem::mouseMoveEvent(event);
    
    // Update connections after moving
    updateConnections();
}

// Get depth level in tree (root=0)
int EditableTextItem::getDepthLevel() const
{
    if (!m_parent_node) {
        return 0; // This is a root node
    }
    
    // Recursively calculate depth by adding 1 to parent's depth
    return m_parent_node->getDepthLevel() + 1;
}

// Calculate total height requirement for this node and its descendants
qreal EditableTextItem::getTotalHeightRequirement() const
{
    // Start with this node's height
    qreal total_height = boundingRect().height() + 20; // Add some vertical spacing
    
    // If no children, just return this node's height
    if (m_child_nodes.isEmpty()) {
        return total_height;
    }
    
    // Calculate sum of all children's height requirements
    qreal children_height = 0;
    for (EditableTextItem *child : m_child_nodes) {
        children_height += child->getTotalHeightRequirement();
    }
    
    // Return the larger of this node's height or its children's total
    return qMax(total_height, children_height);
}

// Organize layout of this node's children
void EditableTextItem::organizeChildrenLayout()
{
    // If no children, nothing to organize
    if (m_child_nodes.isEmpty()) {
        return;
    }
    
    // Position children symmetrically around this node
    positionChildrenSymmetrically();
    
    // Update connections after repositioning
    updateConnections();
    
    // Recursively organize each child's children
    for (EditableTextItem *child : m_child_nodes) {
        child->organizeChildrenLayout();
    }
}

// Position children symmetrically around this node
void EditableTextItem::positionChildrenSymmetrically()
{
    if (m_child_nodes.isEmpty()) {
        return;
    }
    
    // Get this node's position and bounds
    QPointF parent_pos = pos();
    QRectF parent_rect = boundingRect();
    
    // Calculate parent center point
    qreal parent_center_y = parent_pos.y() + parent_rect.height() / 2;
    
    // Standard horizontal distance from parent - all children at same x coordinate
    qreal x_offset = parent_rect.width() + 100; // Increase distance for better visibility
    
    // Special case: if there's only one child, align it horizontally with parent
    if (m_child_nodes.size() == 1) {
        EditableTextItem *child = m_child_nodes.first();
        
        // Position child at same y-level as parent
        child->setPos(parent_pos.x() + x_offset, parent_pos.y());
        
        // Update connection color
        for (ConnectionLine* conn : m_connections) {
            if (conn->targetItem() == child) {
                conn->setColorByIndex(0);
                break;
            }
        }
        return;
    }
    
    // For multiple children, calculate total height
    QList<qreal> child_heights;
    qreal total_height = 0;
    
    for (EditableTextItem *child : m_child_nodes) {
        qreal height = child->boundingRect().height() + 30; // Increased vertical spacing
        child_heights.append(height);
        total_height += height;
    }
    
    // Calculate starting y position to center around parent center point
    qreal start_y = parent_center_y - (total_height / 2);
    
    // Position each child
    for (int i = 0; i < m_child_nodes.size(); ++i) {
        EditableTextItem *child = m_child_nodes[i];
        QRectF child_rect = child->boundingRect();
        
        // Calculate y position for this child, accounting for the child's own height
        qreal y_pos = start_y + (child_rect.height() / 2);
        
        // Update child position - all children aligned on same x coordinate
        child->setPos(parent_pos.x() + x_offset, y_pos);
        
        // Update connection colors
        for (ConnectionLine* conn : m_connections) {
            if (conn->targetItem() == child) {
                conn->setColorByIndex(0);
                break;
            }
        }
        
        // Move to next position
        start_y += child_heights[i];
    }
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
    
    // Process the drop using our universal handler
    if (processItemFromMimeData(event->mimeData(), scene_pos)) {
        event->acceptProposedAction();
    }
}

// Universal handler for mime data (used by both drag-drop and paste)
bool InfiniteCanvas::processItemFromMimeData(const QMimeData *mime_data, const QPointF &pos)
{
    if (!mime_data) {
        return false;
    }
    
    bool handled = false;
    
    // Handle URL data (including shortcuts and directories)
    if (mime_data->hasUrls()) {
        QList<QUrl> urls = mime_data->urls();
        
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                QString file_path = url.toLocalFile();
                QFileInfo file_info(file_path);
                
                // Check if it's a directory
                if (isDirectory(file_path)) {
                    handleDirectoryDrop(url, pos);
                    handled = true;
                    break;
                }
                // Check if it's a Windows shortcut file
                else if (file_info.suffix().toLower() == "lnk") {
                    handleShortcutDrop(url, pos);
                    handled = true;
                    break;
                }
                // Check if it's a media file
                else if (isMediaFile(file_path)) {
                    handleMediaDrop(url, pos);
                    handled = true;
                    break;
                }
            }
        }
        
        // If not handled as a directory, shortcut, or media file, try handling as an image
        if (!handled && (mime_data->hasImage() || mime_data->hasUrls())) {
            handleImageDrop(mime_data, pos);
            handled = true;
        }
    }
    // Handle image data if no URLs or URLs weren't handled
    else if (!handled && mime_data->hasImage()) {
        handleImageDrop(mime_data, pos);
        handled = true;
    }
    
    // Handle text data if nothing else was handled
    if (!handled && mime_data->hasText()) {
        handleTextDrop(mime_data, pos);
        handled = true;
    }
    
    return handled;
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
                // Create custom text item with double-click editing
                EditableTextItem *text_item = new EditableTextItem(text);
                
                // Set font and color
                QFont font("Arial", 12);
                text_item->setFont(font);
                text_item->setDefaultTextColor(Qt::black);
                
                // Set position
                text_item->setPos(pos);
                
                // Add to scene
                scene()->addItem(text_item);
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
        QChar first_char = url.host().at(0).toUpper();
        letter = QString(first_char);
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
    QList<QGraphicsItem*> selected_items = scene()->selectedItems();
    if (!selected_items.isEmpty()) {
        // Check if the selected item is a text node
        if (selected_items.count() == 1 && dynamic_cast<EditableTextItem*>(selected_items.first())) {
            EditableTextItem* text_node = dynamic_cast<EditableTextItem*>(selected_items.first());
            
            // Add "Add Child Node" action to the menu
            QAction *add_child_action = context_menu.addAction(QObject::tr("Add Child Node"));
            connect(add_child_action, &QAction::triggered, [this, text_node]() {
                // Calculate position for new child (below and to the right)
                QPointF child_pos = text_node->pos() + QPointF(50, 50);
                
                // Create child node
                EditableTextItem *child_node = createTextNode(child_pos, "Child Node");
                
                // Add as child
                text_node->addChildNode(child_node);
            });
            
            // Add "Organize Layout" action to the menu
            QAction *organize_action = context_menu.addAction(QObject::tr("Organize Layout"));
            connect(organize_action, &QAction::triggered, [this, text_node]() {
                organizeLayoutFromNode(text_node);
            });
            
            context_menu.addSeparator();
        }
        
        // Add copy action to the menu
        QAction *copy_action = context_menu.addAction(QObject::tr("Copy to Clipboard"));
        connect(copy_action, &QAction::triggered, this, &InfiniteCanvas::copySelectedItemsToClipboard);
        
        // Add delete action to the menu
        QAction *delete_action = context_menu.addAction(QObject::tr("Delete"));
        connect(delete_action, &QAction::triggered, this, &InfiniteCanvas::deleteSelectedItems);
    } else {
        // If no selection, add action to create a new text node
        QAction *new_node_action = context_menu.addAction(QObject::tr("Create New Node"));
        connect(new_node_action, &QAction::triggered, [this, scene_pos]() {
            createTextNode(scene_pos);
        });
    }
    
    // Execute the menu
    context_menu.exec(event->globalPos());
}

// Method to copy selected items to clipboard
void InfiniteCanvas::copySelectedItemsToClipboard()
{
    // Get the list of selected items
    QList<QGraphicsItem*> selected_items = scene()->selectedItems();
    
    if (selected_items.isEmpty()) {
        return;
    }
    
    // If multiple items are selected, only copy the first one
    QGraphicsItem *item = selected_items.first();
    
    // Create a new mime data object
    QMimeData *mime_data = new QMimeData();
    
    // Handle text items
    if (QGraphicsTextItem *text_item = dynamic_cast<QGraphicsTextItem*>(item)) {
        QString text = text_item->toPlainText();
        mime_data->setText(text);
    }
    // Handle URL items
    else if (item->data(1).toString() == "url") {
        QString url_str = item->data(0).toString();
        mime_data->setUrls(QList<QUrl>() << QUrl(url_str));
        mime_data->setText(url_str);
    }
    // Handle shortcut items
    else if (item->data(1).toString() == "shortcut") {
        QString path = item->data(0).toString();
        mime_data->setUrls(QList<QUrl>() << QUrl::fromLocalFile(path));
        mime_data->setText(path);
    }
    // Handle directory items
    else if (item->data(1).toString() == "directory") {
        QString dir_path = item->data(0).toString();
        mime_data->setUrls(QList<QUrl>() << QUrl::fromLocalFile(dir_path));
        mime_data->setText(dir_path);
    }
    // Handle media items
    else if (item->data(1).toString() == "media") {
        QString media_path = item->data(0).toString();
        mime_data->setUrls(QList<QUrl>() << QUrl::fromLocalFile(media_path));
        mime_data->setText(media_path);
    }
    // Handle image items
    else if (QGraphicsPixmapItem *pixmap_item = dynamic_cast<QGraphicsPixmapItem*>(item)) {
        // First check if it has a file path stored
        QVariant path_variant = pixmap_item->data(0);
        if (path_variant.isValid()) {
            QString file_path = path_variant.toString();
            mime_data->setUrls(QList<QUrl>() << QUrl::fromLocalFile(file_path));
            mime_data->setText(file_path);
        }
        
        // Also add the image data
        QPixmap pixmap = pixmap_item->pixmap();
        mime_data->setImageData(pixmap.toImage());
    }
    
    // Set the mime data to the clipboard
    QApplication::clipboard()->setMimeData(mime_data);
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

void InfiniteCanvas::handleMediaDrop(const QUrl &url, const QPointF &pos)
{
    QString media_path = url.toLocalFile();
    
    if (!media_path.isEmpty()) {
        // Get media file icon
        QPixmap icon = getMediaIcon(media_path);
        
        // Create the media item
        MediaItem *media_item = new MediaItem(icon, media_path);
        
        // Position at drop location
        media_item->setPos(pos);
        
        // Add to scene
        scene()->addItem(media_item);
        
        // Set tooltip to show complete media path
        media_item->setToolTip(media_path);
    }
}

// Check if the file is a media file (audio/video)
bool InfiniteCanvas::isMediaFile(const QString &file_path)
{
    QFileInfo file_info(file_path);
    QString suffix = file_info.suffix().toLower();
    
    // Common audio file extensions
    static const QStringList audio_extensions = {
        "mp3", "wav", "ogg", "flac", "aac", "wma", "m4a", "aiff"
    };
    
    // Common video file extensions
    static const QStringList video_extensions = {
        "mp4", "avi", "mov", "wmv", "mkv", "flv", "webm", "m4v", "mpg", "mpeg"
    };
    
    return audio_extensions.contains(suffix) || video_extensions.contains(suffix);
}

// Get media file icon
QPixmap InfiniteCanvas::getMediaIcon(const QString &media_path)
{
    QFileIconProvider icon_provider;
    QFileInfo file_info(media_path);
    QIcon icon = icon_provider.icon(file_info);
    
    // Get a reasonable size for the icon
    QPixmap pixmap = icon.pixmap(64, 64);
    
    // If we got an empty pixmap, use a default
    if (pixmap.isNull()) {
        pixmap = QPixmap(64, 64);
        pixmap.fill(Qt::transparent);
        
        // Draw a media-specific default icon
        QPainter painter(&pixmap);
        
        // Different colors for audio vs video
        QString suffix = file_info.suffix().toLower();
        
        // Audio file extensions
        static const QStringList audio_extensions = {
            "mp3", "wav", "ogg", "flac", "aac", "wma", "m4a", "aiff"
        };
        
        // Video file extensions
        static const QStringList video_extensions = {
            "mp4", "avi", "mov", "wmv", "mkv", "flv", "webm", "m4v", "mpg", "mpeg"
        };
        
        if (audio_extensions.contains(suffix)) {
            // Audio icon (blue)
            painter.setBrush(QColor(100, 149, 237)); // Cornflower blue
            painter.setPen(Qt::black);
            painter.drawEllipse(8, 8, 48, 48);
            
            // Draw audio wave symbol
            QPainterPath path;
            path.moveTo(24, 22);
            path.lineTo(24, 42);
            path.moveTo(32, 18);
            path.lineTo(32, 46);
            path.moveTo(40, 22);
            path.lineTo(40, 42);
            
            painter.setPen(QPen(Qt::white, 3));
            painter.drawPath(path);
        } 
        else if (video_extensions.contains(suffix)) {
            // Video icon (red)
            painter.setBrush(QColor(205, 92, 92)); // Indian red
            painter.setPen(Qt::black);
            painter.drawRect(8, 8, 48, 48);
            
            // Draw video play symbol
            QPolygonF triangle;
            triangle << QPointF(24, 18) << QPointF(44, 32) << QPointF(24, 46);
            
            painter.setBrush(Qt::white);
            painter.setPen(Qt::NoPen);
            painter.drawPolygon(triangle);
        }
    }
    
    return pixmap;
}

void InfiniteCanvas::pasteFromClipboard()
{
    qDebug() << "Pasting from clipboard...";
    
    // Get clipboard
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mime_data = clipboard->mimeData();
    
    if (!mime_data) {
        qWarning() << "Clipboard is empty or inaccessible";
        return;
    }
    
    // Determine paste position
    QPointF paste_pos;
    
    // Get current mouse position in view coordinates
    QPoint mouse_pos = mapFromGlobal(QCursor::pos());
    
    // Check if mouse is over the view
    if (rect().contains(mouse_pos)) {
        // Use mouse position if it's over the graphics view
        paste_pos = mapToScene(mouse_pos);
        qDebug() << "Pasting at mouse position:" << paste_pos;
    } else {
        // Otherwise use the center of the current view
        paste_pos = mapToScene(viewport()->rect().center());
        qDebug() << "Pasting at view center:" << paste_pos;
    }
    
    // Process the clipboard data
    bool success = processItemFromMimeData(mime_data, paste_pos);
    
    if (success) {
        qDebug() << "Successfully pasted item from clipboard";
        
        // Select the last added item (which should be the one we just pasted)
        QList<QGraphicsItem*> all_items = scene()->items();
        if (!all_items.isEmpty()) {
            all_items.first()->setSelected(true);
        }
    } else {
        qWarning() << "Failed to paste - unsupported clipboard format";
    }
}

// Override key press event to handle keyboard shortcuts
void InfiniteCanvas::keyPressEvent(QKeyEvent *event)
{
    // Check for Ctrl+V shortcut
    if ((event->key() == Qt::Key_V) && (event->modifiers() & Qt::ControlModifier)) {
        // Check if there's a text item in edit mode
        QGraphicsItem *focused_item = scene()->focusItem();
        
        // Check for both standard QGraphicsTextItem and our custom EditableTextItem
        QGraphicsTextItem *text_item = qgraphicsitem_cast<QGraphicsTextItem*>(focused_item);
        
        // If a text item is being edited, let the event pass through
        if (text_item && (text_item->textInteractionFlags() & Qt::TextEditorInteraction)) {
            QGraphicsView::keyPressEvent(event);
            return;
        }
        
        // Otherwise perform custom paste operation
        pasteFromClipboard();
        event->accept();
        return;
    }
    
    // Pass other key events to the parent class
    QGraphicsView::keyPressEvent(event);
}

// Public copy method for MainWindow
void InfiniteCanvas::copyToClipboard()
{
    copySelectedItemsToClipboard();
}

// Create a new text node at the specified position
EditableTextItem* InfiniteCanvas::createTextNode(const QPointF &position, const QString &text)
{
    // Create the text item
    EditableTextItem *text_item = new EditableTextItem(text);
    
    // Set font and color
    QFont font("Arial", 12);
    text_item->setFont(font);
    text_item->setDefaultTextColor(Qt::black);
    
    // Set position
    text_item->setPos(position);
    
    // Add to scene
    scene()->addItem(text_item);
    
    // Select the new item
    scene()->clearSelection();
    text_item->setSelected(true);
    
    return text_item;
}

// Organize the entire mind map layout from the selected node
void InfiniteCanvas::organizeLayoutFromNode(EditableTextItem* node)
{
    if (!node) {
        return;
    }
    
    // Organize the layout starting from this node
    node->organizeChildrenLayout();
    
    // Update the scene
    scene()->update();
}

#include "pch.h"

#include "mainwindow.h"
#include "infinitecanvas.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle(tr("QtMindMap"));

  // Create menu bar
  setupMenus();

  // Check if window is already set to stay on top and update the action state
  if (windowFlags() & Qt::WindowStaysOnTopHint) {
    m_always_on_top_action->setChecked(true);
  }

  QWidget *central_widget = new QWidget(this);
  setCentralWidget(central_widget);

  QVBoxLayout *layout = new QVBoxLayout(central_widget);
  layout->setContentsMargins(0, 0, 0, 0);

  m_scene = new QGraphicsScene(this);
  m_scene->setSceneRect(0, 0, 10000, 10000);

  m_graphics_view = new InfiniteCanvas(m_scene, this);
  
  // Initialize view scrollbar position, set the view center to scene position (0,0)
  m_graphics_view->centerOn(0, 0);
  // Set scrollbars to their starting position (left-top corner)
  m_graphics_view->horizontalScrollBar()->setValue(0);
  m_graphics_view->verticalScrollBar()->setValue(0);

  layout->addWidget(m_graphics_view);

  resize(1200, 800);

  QScreen *screen = QApplication::primaryScreen();
  QRect screen_geometry = screen->availableGeometry();
  int x = (screen_geometry.width() - width()) / 2;
  int y = (screen_geometry.height() - height()) / 2;
  move(x, y);

  // Try to load the most recent file
  tryLoadRecentFile();
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
  save_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));  // Add Ctrl+S shortcut

  file_menu->addSeparator();

  // Add Exit action
  QAction *exit_action = new QAction(tr("Exit"), this);
  file_menu->addAction(exit_action);
  connect(exit_action, &QAction::triggered, this, &MainWindow::close);
  exit_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));  // Add Ctrl+W shortcut

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

  // Create View menu
  QMenu *view_menu = menuBar()->addMenu(tr("View"));

  // Add Reset Zoom action
  QAction *reset_zoom_action = new QAction(tr("Reset Zoom (100%)"), this);
  reset_zoom_action->setShortcut(
      QKeySequence(Qt::CTRL + Qt::Key_0));  // Ctrl+0 shortcut
  view_menu->addAction(reset_zoom_action);
  connect(reset_zoom_action, &QAction::triggered, this, &MainWindow::resetZoom);

  // Add Always On Top action
  view_menu->addSeparator();
  m_always_on_top_action = new QAction(tr("Always On Top"), this);
  m_always_on_top_action->setCheckable(true);
  m_always_on_top_action->setChecked(false);
  view_menu->addAction(m_always_on_top_action);
  connect(m_always_on_top_action, &QAction::toggled, this,
          &MainWindow::toggleAlwaysOnTop);

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
  QString file_name = QFileDialog::getOpenFileName(
      this, tr("Open File"), "", tr("JSON Files (*.json);;All Files (*)"));
  if (!file_name.isEmpty()) {
    // Load the file
    loadFromFile(file_name);
    // Save this file as the most recent
    saveRecentFilePath(file_name);
    // Store current file
    m_current_file = file_name;
  }
}

void MainWindow::saveFile() {
  if (!m_current_file.isEmpty()) {
    // Save to the current file
    saveToFile(m_current_file);
  } else {
    // Show save file dialog
    QString file_name = QFileDialog::getSaveFileName(
        this, tr("Save File"), "", tr("JSON Files (*.json);;All Files (*)"));
    if (!file_name.isEmpty()) {
      // Save to the file
      saveToFile(file_name);
      // Save this file as the most recent
      saveRecentFilePath(file_name);
      // Store current file
      m_current_file = file_name;
    }
  }
}

void MainWindow::saveToFile(const QString &file_name) {
  // Create a JSON object to store all data
  QJsonObject json_data;

  // Log file save operation
  qDebug() << "Saving file to:" << file_name;
  
  // Check for empty file name
  if (file_name.isEmpty()) {
    qCritical() << "Error: Empty file name specified for save operation";
    QMessageBox::warning(this, tr("Save Error"),
                         tr("No file name specified."));
    return;
  }

  // Save canvas view state
  QJsonObject view_state;
  view_state["scale_factor"] = m_graphics_view->getScaleFactor();
  QPointF center_point =
      m_graphics_view->mapToScene(m_graphics_view->viewport()->rect().center());
  view_state["center_x"] = center_point.x();
  view_state["center_y"] = center_point.y();
  json_data["view_state"] = view_state;

  // Save items
  QJsonArray items_array;
  
  // Get all top-level items (exclude child items from item groups)
  QList<QGraphicsItem*> all_items = m_scene->items();
  QSet<QGraphicsItem*> processed_items;
  QSet<QGraphicsItem*> child_items;
  
  // First identify all child items of QGraphicsItemGroup to avoid processing them separately
  for (QGraphicsItem *item : all_items) {
    // Check if the item is a QGraphicsItemGroup
    QGraphicsItemGroup *group = dynamic_cast<QGraphicsItemGroup*>(item);
    if (group) {
      // Get all child items in the group
      QList<QGraphicsItem*> children = group->childItems();
      for (QGraphicsItem *child : children) {
        child_items.insert(child);
      }
    }
  }
  
  qDebug() << "Total scene items:" << all_items.size() 
           << "Child items to exclude:" << child_items.size();

  // Iterate through all items in the scene
  for (QGraphicsItem *item : all_items) {
    // Skip if already processed or is a child item
    if (processed_items.contains(item) || child_items.contains(item)) {
      continue;
    }
    
    QJsonObject item_data;

    // Save position for all items
    item_data["x"] = item->pos().x();
    item_data["y"] = item->pos().y();

    // Handle text items
    if (QGraphicsTextItem *text_item =
            dynamic_cast<QGraphicsTextItem *>(item)) {
      item_data["type"] = "text";
      item_data["content"] = text_item->toPlainText();
      item_data["font_family"] = text_item->font().family();
      item_data["font_size"] = text_item->font().pointSize();
      item_data["color"] = text_item->defaultTextColor().name();
      
      items_array.append(item_data);
      processed_items.insert(item);
      qDebug() << "Saved text item at" << item->pos();
    }
    // Handle custom item types
    else if (MediaItem *media_item = dynamic_cast<MediaItem*>(item)) {
      item_data["type"] = "media";
      item_data["media_path"] = media_item->getMediaPath();
      
      items_array.append(item_data);
      processed_items.insert(item);
      
      // Mark all children as processed
      for (QGraphicsItem *child : media_item->childItems()) {
        processed_items.insert(child);
      }
      qDebug() << "Saved media item at" << item->pos();
    }
    else if (DirectoryItem *dir_item = dynamic_cast<DirectoryItem*>(item)) {
      item_data["type"] = "directory";
      item_data["dir_path"] = dir_item->getDirPath();
      
      items_array.append(item_data);
      processed_items.insert(item);
      
      // Mark all children as processed
      for (QGraphicsItem *child : dir_item->childItems()) {
        processed_items.insert(child);
      }
      qDebug() << "Saved directory item at" << item->pos();
    }
    else if (UrlItem *url_item = dynamic_cast<UrlItem*>(item)) {
      item_data["type"] = "url";
      item_data["url"] = url_item->getUrl().toString();
      
      items_array.append(item_data);
      processed_items.insert(item);
      
      // Mark all children as processed
      for (QGraphicsItem *child : url_item->childItems()) {
        processed_items.insert(child);
      }
      qDebug() << "Saved URL item at" << item->pos();
    }
    // Handle shortcut items - not a group but has custom data
    else if (ShortcutItem *shortcut_item = dynamic_cast<ShortcutItem*>(item)) {
      item_data["type"] = "shortcut";
      item_data["target_path"] = shortcut_item->getTargetPath();
      
      items_array.append(item_data);
      processed_items.insert(item);
      qDebug() << "Saved shortcut item at" << item->pos();
    }
    // Alternative detection based on data() for non-dynamic items
    else if (item->data(1).toString() == "shortcut") {
      item_data["type"] = "shortcut";
      item_data["target_path"] = item->data(0).toString();
      
      items_array.append(item_data);
      processed_items.insert(item);
    }
    else if (item->data(1).toString() == "url") {
      item_data["type"] = "url";
      item_data["url"] = item->data(0).toString();
      
      items_array.append(item_data);
      processed_items.insert(item);
    }
    else if (item->data(1).toString() == "directory") {
      item_data["type"] = "directory";
      item_data["dir_path"] = item->data(0).toString();
      
      items_array.append(item_data);
      processed_items.insert(item);
    }
    else if (item->data(1).toString() == "media") {
      item_data["type"] = "media";
      item_data["media_path"] = item->data(0).toString();
      
      items_array.append(item_data);
      processed_items.insert(item);
    }
    // Handle pixmap items (images)
    else if (QGraphicsPixmapItem *pixmap_item =
                 dynamic_cast<QGraphicsPixmapItem *>(item)) {
      // Check if this pixmap is part of a group - should have been handled by parent group
      if (pixmap_item->group()) {
        continue;
      }
      
      item_data["type"] = "image";
      // We need to store image path, but QGraphicsPixmapItem doesn't store it
      // We'll use an object property to store file path when loading an image
      QVariant path_variant = pixmap_item->data(0);
      if (path_variant.isValid() && !path_variant.toString().isEmpty()) {
        item_data["file_path"] = path_variant.toString();
        
        items_array.append(item_data);
        processed_items.insert(item);
        qDebug() << "Saved image item at" << item->pos();
      } else {
        qWarning() << "Skipping image with empty file path at" << item->pos();
      }
    }
    // Skip other item types
    else {
      qDebug() << "Skipping unknown item type at" << item->pos();
      continue;
    }
  }

  json_data["items"] = items_array;
  qDebug() << "Saved" << items_array.size() << "items out of" << all_items.size() << "scene items";

  // Write JSON data to file
  QFile save_file(file_name);
  if (!save_file.open(QIODevice::WriteOnly)) {
    qCritical() << "Failed to open file for writing:" << file_name << "Error:" << save_file.errorString();
    QMessageBox::warning(this, tr("Save Error"),
                         tr("Could not open file for writing."));
    return;
  }

  QJsonDocument save_doc(json_data);
  save_file.write(save_doc.toJson());
  save_file.close();
  
  qDebug() << "Successfully saved file:" << file_name << "Items saved:" << items_array.size();
}

void MainWindow::loadFromFile(const QString &file_name) {
  // Log file load operation
  qDebug() << "Loading file from:" << file_name;
  
  // Check for empty file name
  if (file_name.isEmpty()) {
    qCritical() << "Error: Empty file name specified for load operation";
    QMessageBox::warning(this, tr("Load Error"),
                         tr("No file name specified."));
    return;
  }

  // Check if file exists
  QFileInfo file_info(file_name);
  if (!file_info.exists() || !file_info.isFile()) {
    qCritical() << "Error: File does not exist or is not a regular file:" << file_name;
    QMessageBox::warning(this, tr("Load Error"),
                         tr("File does not exist or is not a regular file."));
    return;
  }

  // Read the JSON file
  QFile load_file(file_name);
  if (!load_file.open(QIODevice::ReadOnly)) {
    qCritical() << "Failed to open file for reading:" << file_name << "Error:" << load_file.errorString();
    QMessageBox::warning(this, tr("Load Error"),
                         tr("Could not open file for reading."));
    return;
  }

  QByteArray file_data = load_file.readAll();
  load_file.close();

  QJsonDocument load_doc(QJsonDocument::fromJson(file_data));
  if (load_doc.isNull() || !load_doc.isObject()) {
    qCritical() << "Invalid JSON format in file:" << file_name;
    QMessageBox::warning(this, tr("Load Error"),
                         tr("File contains invalid JSON data."));
    return;
  }
  
  QJsonObject json_data = load_doc.object();

  // Clear current scene
  m_scene->clear();
  qDebug() << "Scene cleared. Initial item count:" << m_scene->items().count();

  // Restore items
  QJsonArray items_array = json_data["items"].toArray();
  qDebug() << "Loading" << items_array.size() << "items from file";
  
  int logical_items_count = 0; // Count of logical/high-level items
  
  for (int i = 0; i < items_array.size(); ++i) {
    QJsonObject item_data = items_array[i].toObject();
    QString type = item_data["type"].toString();
    QPointF pos(item_data["x"].toDouble(), item_data["y"].toDouble());

    qDebug() << "Processing item" << i+1 << "of" << items_array.size() 
             << "- Type:" << type 
             << "Position:" << pos 
             << "Current scene items:" << m_scene->items().count();

    if (type == "text") {
      // Create text item
      QString content = item_data["content"].toString();
      QGraphicsTextItem *text_item = new QGraphicsTextItem(content);

      // Set font properties if available
      if (item_data.contains("font_family") &&
          item_data.contains("font_size")) {
        QFont font(item_data["font_family"].toString(),
                   item_data["font_size"].toInt());
        text_item->setFont(font);
      }

      // Set color if available
      if (item_data.contains("color")) {
        text_item->setDefaultTextColor(QColor(item_data["color"].toString()));
      }

      // Set position and flags
      text_item->setPos(pos);
      text_item->setFlag(QGraphicsItem::ItemIsSelectable, true);
      text_item->setFlag(QGraphicsItem::ItemIsMovable, true);
      text_item->setTextInteractionFlags(Qt::TextEditorInteraction);

      // Get current count before adding
      int before_count = m_scene->items().count();
      
      // Add to scene
      m_scene->addItem(text_item);
      logical_items_count++;
      
      // Get current count after adding
      int after_count = m_scene->items().count();
      int added_count = after_count - before_count;
      
      qDebug() << "  - Added text item:" << content.left(20) << "..." 
               << "Logical items:" << logical_items_count 
               << "Added" << added_count << "actual scene items"
               << "Total scene items:" << after_count;
    } else if (type == "shortcut") {
      // Create shortcut item
      QString target_path = item_data["target_path"].toString();

      if (!target_path.isEmpty()) {
        // Get icon for the shortcut
        QFileIconProvider icon_provider;
        QFileInfo file_info(target_path);
        QIcon icon = icon_provider.icon(file_info);
        QPixmap pixmap = icon.pixmap(64, 64);

        // If we got an empty pixmap, use a default
        if (pixmap.isNull()) {
          pixmap = QPixmap(64, 64);
          pixmap.fill(Qt::transparent);
        }

        // Get current count before adding
        int before_count = m_scene->items().count();
        
        // Create the shortcut item
        ShortcutItem *shortcut_item = new ShortcutItem(pixmap, target_path);

        // Position at the saved location
        shortcut_item->setPos(pos);

        // Set tooltip to show target path
        shortcut_item->setToolTip(target_path);

        // Add to scene
        m_scene->addItem(shortcut_item);
        logical_items_count++;
        
        // Get current count after adding
        int after_count = m_scene->items().count();
        int added_count = after_count - before_count;
        
        qDebug() << "  - Added shortcut item:" << target_path 
                 << "Logical items:" << logical_items_count 
                 << "Added" << added_count << "actual scene items"
                 << "Total scene items:" << after_count;
      } else {
        qWarning() << "Empty target path for shortcut item at position" << pos;
      }
    } else if (type == "url") {
      // Create URL item
      QString url_str = item_data["url"].toString();

      if (!url_str.isEmpty()) {
        QUrl url(url_str);
        if (url.isValid()) {
          // Get current count before adding
          int before_count = m_scene->items().count();
          
          // Get icon for the website
          QPixmap icon = m_graphics_view->getWebsiteIcon(url);
          
          // Create the URL item
          UrlItem *url_item = new UrlItem(icon, url);
          
          // Position at the saved location
          url_item->setPos(pos);
          
          // Set tooltip to show URL
          url_item->setToolTip(url_str);
          
          // Add to scene
          m_scene->addItem(url_item);
          logical_items_count++;
          
          // Get current count after adding
          int after_count = m_scene->items().count();
          int added_count = after_count - before_count;
          
          qDebug() << "  - Added URL item:" << url_str 
                   << "Logical items:" << logical_items_count 
                   << "Added" << added_count << "actual scene items"
                   << "Total scene items:" << after_count;
        } else {
          qWarning() << "Invalid URL:" << url_str;
        }
      } else {
        qWarning() << "Empty URL for URL item at position" << pos;
      }
    } else if (type == "directory") {
      // Create directory item
      QString dir_path = item_data["dir_path"].toString();

      if (!dir_path.isEmpty()) {
        // Get current count before adding
        int before_count = m_scene->items().count();
        
        // Get icon for the directory
        QFileIconProvider icon_provider;
        QFileInfo dir_info(dir_path);
        QIcon icon = icon_provider.icon(QFileIconProvider::Folder);

        // For specific folder, use its actual icon if it exists
        if (dir_info.exists()) {
          icon = icon_provider.icon(dir_info);
        }

        QPixmap pixmap = icon.pixmap(64, 64);

        // If we got an empty pixmap, use a default
        if (pixmap.isNull()) {
          pixmap = QPixmap(64, 64);
          pixmap.fill(Qt::transparent);
        }

        // Create the directory item with label
        DirectoryItem *dir_item = new DirectoryItem(pixmap, dir_path);

        // Position at the saved location
        dir_item->setPos(pos);

        // Set tooltip to show directory path
        dir_item->setToolTip(dir_path);

        // Add to scene
        m_scene->addItem(dir_item);
        logical_items_count++;
        
        // Get current count after adding
        int after_count = m_scene->items().count();
        int added_count = after_count - before_count;
        
        qDebug() << "  - Added directory item:" << dir_path 
                 << "Logical items:" << logical_items_count 
                 << "Added" << added_count << "actual scene items"
                 << "Total scene items:" << after_count;
      } else {
        qWarning() << "Empty directory path for directory item at position" << pos;
      }
    } else if (type == "media") {
      // Create media item
      QString media_path = item_data["media_path"].toString();

      if (!media_path.isEmpty()) {
        // Get current count before adding
        int before_count = m_scene->items().count();
        
        // Get icon for the media file
        QPixmap icon = m_graphics_view->getMediaIcon(media_path);
        
        // Create the media item
        MediaItem *media_item = new MediaItem(icon, media_path);
        
        // Position at the saved location
        media_item->setPos(pos);
        
        // Set tooltip to show media path
        media_item->setToolTip(media_path);
        
        // Add to scene
        m_scene->addItem(media_item);
        logical_items_count++;
        
        // Get current count after adding
        int after_count = m_scene->items().count();
        int added_count = after_count - before_count;
        
        qDebug() << "  - Added media item:" << media_path 
                 << "Logical items:" << logical_items_count 
                 << "Added" << added_count << "actual scene items"
                 << "Total scene items:" << after_count;
      } else {
        qWarning() << "Empty media path for media item at position" << pos;
      }
    } else if (type == "image") {
      // Load image from file path
      QString file_path = item_data["file_path"].toString();
      
      if (file_path.isEmpty()) {
        qWarning() << "Empty file path for image item at position" << pos;
        continue;
      }
      
      QFileInfo img_file_info(file_path);
      if (!img_file_info.exists()) {
        qWarning() << "Image file does not exist:" << file_path;
        continue;
      }
      
      qDebug() << "  - Loading image file:" << file_path;
      QImage image(file_path);

      if (!image.isNull()) {
        // Get current count before adding
        int before_count = m_scene->items().count();
        
        QGraphicsPixmapItem *pixmap_item =
            new QGraphicsPixmapItem(QPixmap::fromImage(image));
        pixmap_item->setPos(pos);
        pixmap_item->setFlag(QGraphicsItem::ItemIsSelectable, true);
        pixmap_item->setFlag(QGraphicsItem::ItemIsMovable, true);

        // Store the file path for later saving
        pixmap_item->setData(0, file_path);

        // Add to scene
        m_scene->addItem(pixmap_item);
        logical_items_count++;
        
        // Get current count after adding
        int after_count = m_scene->items().count();
        int added_count = after_count - before_count;
        
        qDebug() << "  - Added image item:" << file_path 
                 << "Logical items:" << logical_items_count 
                 << "Added" << added_count << "actual scene items"
                 << "Total scene items:" << after_count;
      } else {
        qWarning() << "Failed to load image from:" << file_path;
      }
    } else {
      qWarning() << "Unknown item type:" << type << "at position" << pos;
    }
  }

  int actual_item_count = m_scene->items().count();
  qDebug() << "Successfully loaded" << logical_items_count << "logical items out of" << items_array.size() << "items from file";
  qDebug() << "Final scene item count:" << actual_item_count << "(includes group component items)";
  
  // Explain any discrepancy between logical items and actual scene items
  if (logical_items_count != items_array.size()) {
    qWarning() << "LOGICAL ITEMS DISCREPANCY: Loaded" << logical_items_count 
               << "logical items but the file contained" << items_array.size() << "items";
  }
  
  if (logical_items_count != actual_item_count) {
    qDebug() << "NOTE: The difference between logical items (" << logical_items_count 
             << ") and scene items (" << actual_item_count 
             << ") is expected due to item groups (like MediaItem, DirectoryItem, etc.)";
    
    // List all items for debugging if requested
    bool list_all_items = false;  // Set to true to enable detailed listing
    if (list_all_items) {
      int idx = 0;
      foreach (QGraphicsItem *item, m_scene->items()) {
        QString itemType = "Unknown";
        
        if (dynamic_cast<QGraphicsTextItem*>(item))
          itemType = "Text";
        else if (dynamic_cast<ShortcutItem*>(item))
          itemType = "Shortcut";
        else if (dynamic_cast<UrlItem*>(item))
          itemType = "URL";
        else if (dynamic_cast<DirectoryItem*>(item))
          itemType = "Directory";
        else if (dynamic_cast<MediaItem*>(item))
          itemType = "Media";
        else if (dynamic_cast<QGraphicsPixmapItem*>(item))
          itemType = "Image";
        else if (dynamic_cast<QGraphicsSimpleTextItem*>(item))
          itemType = "Label";
        else if (dynamic_cast<QGraphicsItemGroup*>(item))
          itemType = "Group";
        
        qDebug() << "  Scene item" << ++idx << ":" << itemType << "at" << item->pos();
      }
    }
  }

  // Restore view state
  if (json_data.contains("view_state")) {
    QJsonObject view_state = json_data["view_state"].toObject();

    // Restore scale factor
    double scale_factor = view_state["scale_factor"].toDouble();
    m_graphics_view->resetTransform();
    m_graphics_view->scale(scale_factor, scale_factor);
    m_graphics_view->setScaleFactor(scale_factor);

    // Restore view center position
    if (view_state.contains("center_x") && view_state.contains("center_y")) {
      double center_x = view_state["center_x"].toDouble();
      double center_y = view_state["center_y"].toDouble();
      m_graphics_view->centerOn(center_x, center_y);
    }
    
    qDebug() << "Restored view state: scale =" << scale_factor
             << "center =(" << view_state["center_x"].toDouble()
             << "," << view_state["center_y"].toDouble() << ")";
  } else {
    qDebug() << "No view state found in file, using defaults";
  }
  
  qDebug() << "File loading complete:" << file_name;
}

void MainWindow::showAbout() {
  QMessageBox::about(this, tr("About QtMindMap"),
                     tr("QtMindMap is a simple mind mapping application.\n"
                        "Supports drag and drop of images and text."));
}

MainWindow::~MainWindow() {}

// Method to save the most recent file path to an INI file
void MainWindow::saveRecentFilePath(const QString &file_path) {
  QString settings_path = getSettingsFilePath();
  QSettings settings(settings_path, QSettings::IniFormat);
  settings.setValue("RecentFile/Path", file_path);
}

// Method to load the most recent file path from an INI file
QString MainWindow::loadRecentFilePath() {
  QString settings_path = getSettingsFilePath();
  QSettings settings(settings_path, QSettings::IniFormat);
  return settings.value("RecentFile/Path", "").toString();
}

// Get the settings file path in the AppData directory
QString MainWindow::getSettingsFilePath() {
  // Get the AppData location
  QString app_data_path =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

  // Create the directory if it doesn't exist
  QDir app_dir(app_data_path);
  if (!app_dir.exists()) {
    app_dir.mkpath(".");
  }

  // Return the settings file path
  return app_data_path + "/qtmindmap_settings.ini";
}

// Try to load the most recent file
void MainWindow::tryLoadRecentFile() {
  QString recent_file = loadRecentFilePath();
  if (!recent_file.isEmpty()) {
    QFile file(recent_file);
    if (file.exists()) {
      loadFromFile(recent_file);
      m_current_file = recent_file;
    }
  }
}

// Reset zoom to 100%
void MainWindow::resetZoom() {
  if (m_graphics_view) {
    m_graphics_view->resetZoom();
  }
}

// Toggle window always on top
void MainWindow::toggleAlwaysOnTop(bool checked) {
#ifdef Q_OS_WIN
  /*
   * Using Windows-specific SetWindowPos API to set topmost state instead of Qt's setWindowFlags:
   * 
   * Benefits:
   * 1. Avoids window flickering - setWindowFlags would destroy and recreate the window
   * 2. Better user experience - the window doesn't flash or lose focus
   * 3. Preserves window state - maximized/minimized state is maintained
   * 4. Performance - no need to rebuild the entire window
   * 5. No redrawing needed - the window content doesn't need to be repainted
   */
  
  // Get native window handle from Qt window
  HWND hwnd = (HWND)winId();

  if (checked) {
    // Set window to be topmost (always on top of other windows)
    // HWND_TOPMOST: Place the window above all non-topmost windows
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE |  // Don't change the position
                 SWP_NOSIZE |  // Don't change the size
                 SWP_NOACTIVATE); // Don't activate the window
  } else {
    // Set window to be non-topmost (normal z-order)
    // HWND_NOTOPMOST: Place the window above all non-topmost windows
    // (i.e., behind all topmost windows)
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE |  // Don't change the position
                 SWP_NOSIZE |  // Don't change the size
                 SWP_NOACTIVATE); // Don't activate the window
  }
#else
  // For other platforms, fall back to the standard method
  // Note: This method causes flickering as it rebuilds the window
  bool was_maximized = isMaximized();
  bool was_minimized = isMinimized();

  Qt::WindowFlags flags = window()->windowFlags();

  if (checked) {
    window()->setWindowFlags(flags | Qt::WindowStaysOnTopHint);
  } else {
    window()->setWindowFlags(flags & ~Qt::WindowStaysOnTopHint);
  }

  // Restore window to its previous state after rebuilding
  if (was_minimized) {
    showMinimized();
  } else if (was_maximized) {
    showMaximized();
  } else {
    showNormal();
  }
#endif
}

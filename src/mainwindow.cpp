#include "mainwindow.h"

#include <QApplication>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QScreen>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QtMath>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QIcon>
#include <QPixmap>
#include <QKeySequence>

#include "infinitecanvas.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle(tr("QtMindMap"));

  // Create menu bar
  setupMenus();

  QWidget *central_widget = new QWidget(this);
  setCentralWidget(central_widget);

  QVBoxLayout *layout = new QVBoxLayout(central_widget);
  layout->setContentsMargins(0, 0, 0, 0);

  m_scene = new QGraphicsScene(this);
  m_scene->setSceneRect(-5000, -5000, 10000, 10000);

  m_graphics_view = new InfiniteCanvas(m_scene, this);

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
  
  file_menu->addSeparator();
  
  // Add Exit action
  QAction *exit_action = new QAction(tr("Exit"), this);
  file_menu->addAction(exit_action);
  connect(exit_action, &QAction::triggered, this, &MainWindow::close);
  
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
  reset_zoom_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_0));  // Ctrl+0 shortcut
  view_menu->addAction(reset_zoom_action);
  connect(reset_zoom_action, &QAction::triggered, this, &MainWindow::resetZoom);
  
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
  QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("JSON Files (*.json);;All Files (*)"));
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
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("JSON Files (*.json);;All Files (*)"));
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
  
  // Save canvas view state
  QJsonObject view_state;
  view_state["scale_factor"] = m_graphics_view->getScaleFactor();
  QPointF center_point = m_graphics_view->mapToScene(m_graphics_view->viewport()->rect().center());
  view_state["center_x"] = center_point.x();
  view_state["center_y"] = center_point.y();
  json_data["view_state"] = view_state;
  
  // Save items
  QJsonArray items_array;
  
  // Iterate through all items in the scene
  foreach (QGraphicsItem *item, m_scene->items()) {
    QJsonObject item_data;
    
    // Save position for all items
    item_data["x"] = item->pos().x();
    item_data["y"] = item->pos().y();
    
    // Handle text items
    if (QGraphicsTextItem *text_item = dynamic_cast<QGraphicsTextItem*>(item)) {
      item_data["type"] = "text";
      item_data["content"] = text_item->toPlainText();
      item_data["font_family"] = text_item->font().family();
      item_data["font_size"] = text_item->font().pointSize();
      item_data["color"] = text_item->defaultTextColor().name();
    }
    // Handle shortcut items
    else if (item->data(1).toString() == "shortcut") {
      item_data["type"] = "shortcut";
      item_data["target_path"] = item->data(0).toString();
    }
    // Handle directory items
    else if (item->data(1).toString() == "directory") {
      item_data["type"] = "directory";
      item_data["dir_path"] = item->data(0).toString();
    }
    // Handle pixmap items (images)
    else if (QGraphicsPixmapItem *pixmap_item = dynamic_cast<QGraphicsPixmapItem*>(item)) {
      item_data["type"] = "image";
      // We need to store image path, but QGraphicsPixmapItem doesn't store it
      // We'll use an object property to store file path when loading an image
      QVariant path_variant = pixmap_item->data(0);
      if (path_variant.isValid()) {
        item_data["file_path"] = path_variant.toString();
      }
    }
    // Skip other item types
    else {
      continue;
    }
    
    items_array.append(item_data);
  }
  
  json_data["items"] = items_array;
  
  // Write JSON data to file
  QFile save_file(file_name);
  if (!save_file.open(QIODevice::WriteOnly)) {
    QMessageBox::warning(this, tr("Save Error"),
                         tr("Could not open file for writing."));
    return;
  }
  
  QJsonDocument save_doc(json_data);
  save_file.write(save_doc.toJson());
  save_file.close();
}

void MainWindow::loadFromFile(const QString &file_name) {
  // Read the JSON file
  QFile load_file(file_name);
  if (!load_file.open(QIODevice::ReadOnly)) {
    QMessageBox::warning(this, tr("Load Error"),
                         tr("Could not open file for reading."));
    return;
  }
  
  QByteArray file_data = load_file.readAll();
  load_file.close();
  
  QJsonDocument load_doc(QJsonDocument::fromJson(file_data));
  QJsonObject json_data = load_doc.object();
  
  // Clear current scene
  m_scene->clear();
  
  // Restore items
  QJsonArray items_array = json_data["items"].toArray();
  for (int i = 0; i < items_array.size(); ++i) {
    QJsonObject item_data = items_array[i].toObject();
    QString type = item_data["type"].toString();
    QPointF pos(item_data["x"].toDouble(), item_data["y"].toDouble());
    
    if (type == "text") {
      // Create text item
      QString content = item_data["content"].toString();
      QGraphicsTextItem *text_item = new QGraphicsTextItem(content);
      
      // Set font properties if available
      if (item_data.contains("font_family") && item_data.contains("font_size")) {
        QFont font(item_data["font_family"].toString(), item_data["font_size"].toInt());
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
      
      m_scene->addItem(text_item);
    }
    else if (type == "shortcut") {
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
        
        // Create the shortcut item
        ShortcutItem *shortcut_item = new ShortcutItem(pixmap, target_path);
        
        // Position at the saved location
        shortcut_item->setPos(pos);
        
        // Set tooltip to show target path
        shortcut_item->setToolTip(target_path);
        
        // Add to scene
        m_scene->addItem(shortcut_item);
      }
    }
    else if (type == "directory") {
      // Create directory item
      QString dir_path = item_data["dir_path"].toString();
      
      if (!dir_path.isEmpty()) {
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
      }
    }
    else if (type == "image") {
      // Load image from file path
      QString file_path = item_data["file_path"].toString();
      QImage image(file_path);
      
      if (!image.isNull()) {
        QGraphicsPixmapItem *pixmap_item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
        pixmap_item->setPos(pos);
        pixmap_item->setFlag(QGraphicsItem::ItemIsSelectable, true);
        pixmap_item->setFlag(QGraphicsItem::ItemIsMovable, true);
        
        // Store the file path for later saving
        pixmap_item->setData(0, file_path);
        
        m_scene->addItem(pixmap_item);
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
  }
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
    QString app_data_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    
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

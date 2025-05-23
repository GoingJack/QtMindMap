#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "pch.h"

class InfiniteCanvas;
class ShortcutItem;
class UrlItem;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void newFile();
  void openFile();
  void saveFile();
  void showAbout();
  void resetZoom();
  void toggleAlwaysOnTop(bool checked);
  void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
  void showMainWindow();
  void hideMainWindow();
  void changeLanguage(const QString &locale_code);

 protected:
  void closeEvent(QCloseEvent *event) override;
  void changeEvent(QEvent *event) override;
  void exportToPng();
  void exportToPdf();

 private:
  void setupMenus();
  void setupTrayIcon();
  void saveToFile(const QString &file_name);
  void loadFromFile(const QString &file_name);

  // Recent file handling
  void saveRecentFilePath(const QString &file_path);
  QString loadRecentFilePath();
  QString getSettingsFilePath();
  void tryLoadRecentFile();

  // Language settings
  void saveLanguageSetting(const QString &locale_code);
  QString loadLanguageSetting();

  // Window title handling
  void updateWindowTitle();

  InfiniteCanvas *m_graphics_view;
  QGraphicsScene *m_scene;
  QString m_current_file;
  QAction *m_always_on_top_action;

  // Tray icon related
  QSystemTrayIcon *m_tray_icon;
  QMenu *m_tray_menu;
  bool m_tray_message_shown;  // Flag to track if tray message has been shown

  // Language handling
  void setupLanguageMenu();
  void loadLanguage(const QString &locale_code);

  // Language related
  QTranslator *m_translator;
  QString m_current_lang;
  QMenu *m_lang_menu;
};

#endif  // MAINWINDOW_H

#ifndef PCH_H
#define PCH_H

// Standard C++ headers
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <functional>

// Qt Core
#include <QObject>
#include <QString>
#include <QList>
#include <QVector>
#include <QMap>
#include <QSet>
#include <QHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QTimer>
#include <QSettings>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <QtMath>
#include <QProcess>
#include <QCoreApplication>

// Qt GUI
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <QScreen>
#include <QPainter>
#include <QColor>
#include <QFont>
#include <QFileIconProvider>
#include <QFileDialog>
#include <QMessageBox>
#include <QWheelEvent>
#include <QKeySequence>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QClipboard>
#include <QImageReader>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QTextCursor>

// Qt Graphics
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsItemGroup>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSimpleTextItem>

// Qt Network
#include <QUrl>
#include <QDesktopServices>

// Qt Drag & Drop
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

// Qt Print Support
#include <QPrinter>

// Qt Regular Expressions
#include <QRegularExpression>

// Windows specific
#ifdef Q_OS_WIN
#include <Windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#include <shobjidl.h>
#include <objidl.h>
#endif

#endif // PCH_H 

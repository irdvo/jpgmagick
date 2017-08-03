#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QModelIndex>

class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
class QLabel;
class QScrollArea;
class QFileDialog;
class QScrollBar;
class QTableView;
class QListView;
class QFileSystemModel;
class QDockWidget;
class QPushButton;
class QLineEdit;
class QPlainTextEdit;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(int argc, char *argv[]);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void openDirectory();
  void parentDirectory();
  void about();
  void zoomIn();
  void zoomOut();
  void setNormalSize();
  void setFullSize();
  void fitToWindow();
  void selectInDirectory(const QModelIndex &);
  void directoryLoaded(const QString &);
  void rootPathChanged(const QString &);
  void selectPrevImage();
  void selectNextImage();

private:
  void createCentralWidget();
  void createActions();
  void createMenus();
  void createToolBars();
  void createDirectoryDock();

  void setTitle();
  void updateActions();

  void adjustScrollBar(QScrollBar *scrollBar, double factor);

  void openImage(const QString &filename);

  void setImage(const QImage &image);
  void scaleImage(double factor);

  void restoreSettings();
  void saveSettings();

  QModelIndex getFirstIndex();
  void deselectDirectorySelections();
  void selectFirstImage();
  void selectCurrentImage();

  QMenu            *_fileMenu;
  QMenu            *_imageMenu;
  QMenu            *_windowMenu;
  QMenu            *_helpMenu;
  QToolBar         *_toolBar;

  QAction          *_openDirectoryAction;
  QAction          *_parentDirectoryAction;
  QAction          *_quitAction;
  QAction          *_aboutAction;
  QAction          *_aboutQtAction;

  QAction          *_zoomInAction;
  QAction          *_zoomOutAction;
  QAction          *_setNormalSizeAction;
  QAction          *_setFullSizeAction;
  QAction          *_fitToWindowAction;

  QAction          *_prevImageAction;
  QAction          *_nextImageAction;

  // Directory dock
  QDockWidget      *_directoryDock;
  QListView        *_directoryView;
  QFileSystemModel *_fileSystemModel;

  // Image
  QLabel           *_imageLabel;
  QScrollArea      *_imageScrollArea;
  QString           _imageFilename;
  QString           _imagePath;
  QImage            _image;
  double            _scaleFactor;
};

#endif

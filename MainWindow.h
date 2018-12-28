#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QModelIndex>

#include "Magick.h"

class QAction;
class QListWidget;
class QMenu;
class QLabel;
class QScrollArea;
class QFileDialog;
class QScrollBar;
class QTableView;
class FileView;
class QFileSystemModel;
class QDockWidget;
class QPushButton;
class QTabWidget;

class ContrastBrightnessTab;

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
  void saveConverted();
  void parentDirectory();
  void about();
  void zoomIn();
  void zoomOut();
  void setNormalSize();
  void setFullSize();
  void fitToWindow();
  void indexSelected(const QModelIndex &);
  void indexActivated(const QModelIndex &);
  void directoryLoaded(const QString &);
  void rootPathChanged(const QString &);
  void moveToPrevImage();
  void moveToNextImage();
  void doConvertImage();
  void imageConverted();

private:
  void createCentralWidget();
  void createActions();
  void createMenus();
  void createToolBars();
  void createDirectoryDock();

  void setTitle();
  void updateActions();

  void adjustScrollBar(QScrollBar *scrollBar, double factor);

  void setImage1(const QString &filename);
  void clearImage1();
  void clearImage2();

  bool openImage(const QString &filename, QLabel *label, QImage &image);
  bool openImage2(const QString &filename, QImage &image);

  void scaleImages(double factor);
  void sizeImages();

  void drawImages(QLabel *label, const QImage &image1, const QImage &image2);

  void restoreSettings();
  void saveSettings();

  QModelIndex getFirstIndex();
  void deselectDirectorySelections();
  void moveToFirstImage();
  void moveToCurrentImage();
  void moveToIndex(const QModelIndex &);

  QMenu            *_fileMenu;
  QMenu            *_imageMenu;
  QMenu            *_windowMenu;
  QMenu            *_helpMenu;
  QToolBar         *_toolBar;

  QAction          *_openDirectoryAction;
  QAction          *_saveAction;
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
  FileView         *_directoryView;
  QFileSystemModel *_fileSystemModel;

  // Image
  QLabel           *_imageLabel;
  QScrollArea      *_imageScrollArea;
  QString           _imageFilename;
  QImage            _image1;
  QImage            _image2;
  QString           _imagePath;
  double            _scaleFactor;
  QString           _tempFilename;

  // Tab
  QTabWidget       *_actionTab;

  ContrastBrightnessTab *_contrastBrightnessTab;

  // Magick
  Magick            _magick;
};

#endif

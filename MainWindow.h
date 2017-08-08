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
  void selectInDirectory(const QModelIndex &);
  void directoryLoaded(const QString &);
  void rootPathChanged(const QString &);
  void selectPrevImage();
  void selectNextImage();
  void doConvert();
  void converted();

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
  void scaleImages(double factor);
  void sizeImages();

  void restoreSettings();
  void saveSettings();

  QModelIndex getFirstIndex();
  void deselectDirectorySelections();
  void selectFirstImage();
  void selectCurrentImage();

  void clearConvertedImage();
  void setConvertedImage(const QImage &image);

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
  QLabel           *_image1Label;
  QScrollArea      *_image1ScrollArea;
  QLabel           *_image2Label;
  QScrollArea      *_image2ScrollArea;
  QString           _imageFilename;
  QString           _imagePath;
  QImage            _image;
  double            _scaleFactor;

  QString           _tempFilename;

  // Tab
  QTabWidget       *_actionTab;

  ContrastBrightnessTab *_contrastBrightnessTab;

  // Magick
  Magick            _magick;
};

#endif

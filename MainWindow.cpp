#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "MainWindow.h"
#include "FileView.h"
#include "ContrastBrightnessTab.h"

MainWindow::MainWindow(int argc, char *argv[]) :
  _imagePath(QDir::currentPath()),
  _scaleFactor(1.0)
{
  QCoreApplication::setOrganizationName("jpgmagick");
  QCoreApplication::setApplicationName ("jpgmagick");

  if (argc > 1)
  {
    if (QFileInfo(argv[1]).isDir())
    {
      _imagePath = argv[1];
    }
  }

  _tempFilename = QString("%1/jpgmagick.jpg").arg(QDir::tempPath());

#if QT_VERSION >= 0x050000
  connect(&_magick, &Magick::converted, this, &MainWindow::imageConverted);
#else
  connect(&_magick, SIGNAL(converted()), this, SLOT(imageConverted()));
#endif

  createCentralWidget();
  createActions();
  createMenus();
  createToolBars();
  createDirectoryDock();

  restoreSettings();

  setTitle();
  updateActions();
}

MainWindow::~MainWindow()
{

}

// == GUI =====================================================================
void MainWindow::createCentralWidget()
{
  QVBoxLayout *vbox = new QVBoxLayout;

    _imageLabel = new QLabel;
      _imageLabel->setBackgroundRole(QPalette::Base);
      _imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
      _imageLabel->setScaledContents(true);
      _imageLabel->setAlignment(Qt::AlignCenter);
      _imageLabel->setPixmap(QPixmap());

    _imageScrollArea = new QScrollArea;
      _imageScrollArea->setBackgroundRole(QPalette::Base);
      _imageScrollArea->setWidget(_imageLabel);
      _imageScrollArea->setVisible(true);
      _imageScrollArea->setAlignment(Qt::AlignCenter);

    vbox->addWidget(_imageScrollArea, 1);

    _actionTab = new QTabWidget;

      _contrastBrightnessTab = new ContrastBrightnessTab();
#if QT_VERSION >= 0x050000
        connect(_contrastBrightnessTab, &ContrastBrightnessTab::doConvert, this, &MainWindow::doConvertImage);
        connect(_contrastBrightnessTab, &ContrastBrightnessTab::nextImage, this, &MainWindow::moveToNextImage);
#else
        connect(_contrastBrightnessTab, SIGNAL(doConvert()), this, SLOT(doConvertImage()));
        connect(_contrastBrightnessTab, SIGNAL(nextImage()), this, SLOT(moveToNextImage()));
#endif
      _actionTab->addTab(_contrastBrightnessTab, tr("Contrast && Brightness"));

    vbox->addWidget(_actionTab);

  QWidget *widget = new QWidget(this);
  widget->setLayout(vbox);

  setCentralWidget(widget);
}

void MainWindow::createActions()
{
  QCommonStyle *style = new QCommonStyle;

  QIcon openDirectoryIcon = style->standardIcon(QStyle::SP_DirOpenIcon);
  _openDirectoryAction = new QAction(openDirectoryIcon, tr("&Open Directory..."), this);
  _openDirectoryAction->setStatusTip(tr("Open a directory with image files"));
#if QT_VERSION >= 0x050000
  connect(_openDirectoryAction, &QAction::triggered, this, &MainWindow::openDirectory);
#else
  connect(_openDirectoryAction, SIGNAL(triggered()), this, SLOT(openDirectory()));
#endif

  QIcon saveIcon = style->standardIcon(QStyle::SP_DialogSaveButton);
  _saveAction = new QAction(saveIcon, tr("&Save converted image..."), this);
  _saveAction->setStatusTip(tr("Save converted image"));
  _saveAction->setEnabled(false);
#if QT_VERSION >= 0x050000
  connect(_saveAction, &QAction::triggered, this, &MainWindow::saveConverted);
#else
  connect(_saveAction, SIGNAL(triggered()), this, SLOT(saveConverted()));
#endif

  QIcon parentDirectoryIcon = style->standardIcon(QStyle::SP_FileDialogToParent);
  _parentDirectoryAction = new QAction(parentDirectoryIcon, tr("&Goto Parent Directory"), this);
  _parentDirectoryAction->setStatusTip(tr("Goto the parent directory"));
#if QT_VERSION >= 0x050000
  connect(_parentDirectoryAction, &QAction::triggered, this, &MainWindow::parentDirectory);
#else
  connect(_parentDirectoryAction, SIGNAL(triggered()), this, SLOT(parentDirectory()));
#endif

  _quitAction = new QAction(tr("&Quit"), this);
  _quitAction->setShortcuts(QKeySequence::Quit);
  _quitAction->setStatusTip(tr("Quit the application"));
#if QT_VERSION >= 0x050000
  connect(_quitAction, &QAction::triggered, this, &MainWindow::close);
#else
  connect(_quitAction, SIGNAL(triggered()), this, SLOT(close()));
#endif

  _zoomInAction = new QAction(tr("Zoom &In (25%)"), this);
  _zoomInAction->setShortcut(QKeySequence::ZoomIn);
  _zoomInAction->setEnabled(false);
#if QT_VERSION >= 0x050000
  connect(_zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);
#else
  connect(_zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));
#endif

  _zoomOutAction = new QAction(tr("Zoom &Out (25%)"), this);
  _zoomOutAction->setShortcut(QKeySequence::ZoomOut);
  _zoomOutAction->setEnabled(false);
#if QT_VERSION >= 0x050000
  connect(_zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);
#else
  connect(_zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));
#endif

  _setNormalSizeAction = new QAction(tr("&Normal Size"), this);
  _setNormalSizeAction->setShortcut(tr("Ctrl+S"));
  _setNormalSizeAction->setEnabled(false);
#if QT_VERSION >= 0x050000
  connect(_setNormalSizeAction, &QAction::triggered, this, &MainWindow::setNormalSize);
#else
  connect(_setNormalSizeAction, SIGNAL(triggered()), this, SLOT(setNormalSize()));
#endif

  _setFullSizeAction = new QAction(tr("&Full Size"), this);
  _setFullSizeAction->setShortcut(tr("Ctrl+F"));
  _setFullSizeAction->setEnabled(false);
#if QT_VERSION >= 0x050000
  connect(_setFullSizeAction, &QAction::triggered, this, &MainWindow::setFullSize);
#else
  connect(_setFullSizeAction, SIGNAL(triggered()), this, SLOT(setFullSize()));
#endif

  _fitToWindowAction = new QAction(tr("&Fit to Window"), this);
  _fitToWindowAction->setCheckable(true);
  _fitToWindowAction->setShortcut(tr("Ctrl+W"));
  _fitToWindowAction->setEnabled(false);
#if QT_VERSION >= 0x050000
  connect(_fitToWindowAction, &QAction::triggered, this, &MainWindow::fitToWindow);
#else
  connect(_fitToWindowAction, SIGNAL(triggered()), this, SLOT(fitToWindow()));
#endif

  QIcon prevImageIcon = style->standardIcon(QStyle::SP_ArrowBack);
  _prevImageAction = new QAction(prevImageIcon, tr("Show prevous image"), this);
#if QT_VERSION >= 0x050000
  connect(_prevImageAction, &QAction::triggered, this, &MainWindow::moveToPrevImage);
#else
  connect(_prevImageAction, SIGNAL(triggered()), this, SLOT(moveToPrevImage()));
#endif

  QIcon nextImageIcon = style->standardIcon(QStyle::SP_ArrowForward);
  _nextImageAction = new QAction(nextImageIcon, tr("Show next image"), this);
#if QT_VERSION >= 0x050000
  connect(_nextImageAction, &QAction::triggered, this, &MainWindow::moveToNextImage);
#else
  connect(_nextImageAction, SIGNAL(triggered()), this, SLOT(moveToNextImage()));
#endif

  _aboutAction = new QAction(tr("&About"), this);
  _aboutAction->setStatusTip(tr("Show the application's About box"));
#if QT_VERSION >= 0x050000
  connect(_aboutAction, &QAction::triggered, this, &MainWindow::about);
#else
  connect(_aboutAction, SIGNAL(triggered()), this, SLOT(about()));
#endif

  _aboutQtAction = new QAction(tr("About &Qt"), this);
  _aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
#if QT_VERSION >= 0x050000
  connect(_aboutQtAction, &QAction::triggered, this, &QApplication::aboutQt);
#else
  connect(_aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
#endif
}

void MainWindow::createMenus()
{
  _fileMenu = menuBar()->addMenu(tr("&File"));
  _fileMenu->addAction(_openDirectoryAction);
  _fileMenu->addSeparator();
  _fileMenu->addAction(_saveAction);
  _fileMenu->addSeparator();
  _fileMenu->addAction(_quitAction);

  _imageMenu = menuBar()->addMenu(tr("&Image"));

  _imageMenu->addAction(_zoomInAction);
  _imageMenu->addAction(_zoomOutAction);
  _imageMenu->addAction(_setNormalSizeAction);
  _imageMenu->addAction(_setFullSizeAction);
  _imageMenu->addAction(_fitToWindowAction);
  _fileMenu->addSeparator();

  _windowMenu = menuBar()->addMenu(tr("&Window"));

  _helpMenu = menuBar()->addMenu(tr("&Help"));
  _helpMenu->addAction(_aboutAction);
  _helpMenu->addAction(_aboutQtAction);
}

void MainWindow::createToolBars()
{
  _toolBar = addToolBar("");
  _toolBar->setObjectName("ToolBar");
  _toolBar->addAction(_openDirectoryAction);
  _toolBar->addAction(_parentDirectoryAction);
  _toolBar->addSeparator();
  _toolBar->addAction(_prevImageAction);
  _toolBar->addAction(_nextImageAction);
  _toolBar->addSeparator();
  _toolBar->addAction(_saveAction);
}

void MainWindow::createDirectoryDock()
{
  _directoryDock = new QDockWidget(tr("Images: %1").arg(QDir::currentPath()), this);
    _directoryDock->setObjectName("Images");
    _directoryDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

     _fileSystemModel = new QFileSystemModel(this);
     _fileSystemModel->setFilter(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);

     QStringList fileSystemFilters; fileSystemFilters << "*.jpg" << "*.JPG" << "*.jpeg" << "*.JPEG";

     _fileSystemModel->setNameFilters(fileSystemFilters);
     _fileSystemModel->setNameFilterDisables(false);
     _fileSystemModel->setRootPath(_imagePath);
#if QT_VERSION >= 0x050000
     connect(_fileSystemModel, &QFileSystemModel::directoryLoaded, this, &MainWindow::directoryLoaded);
     connect(_fileSystemModel, &QFileSystemModel::rootPathChanged, this, &MainWindow::rootPathChanged);
#else
     connect(_fileSystemModel, SIGNAL(directoryLoaded(const QString&)), this, SLOT(directoryLoaded(const QString&)));
     connect(_fileSystemModel, SIGNAL(rootPathChanged(const QString&)), this, SLOT(rootPathChanged(const QString&)));
#endif

     _directoryView = new FileView(_directoryDock);
     _directoryView->setModel(_fileSystemModel);
     _directoryView->setRootIndex(_fileSystemModel->index(_imagePath));
#if QT_VERSION >= 0x050000
     connect(_directoryView, &QListView::activated,    this, &MainWindow::indexActivated);
     connect(_directoryView, &FileView::indexSelected, this, &MainWindow::indexSelected);
#else
     connect(_directoryView, SIGNAL(activated(const QModelIndex&)),     this, SLOT(indexActivated(const QModelIndex&)));
     connect(_directoryView, SIGNAL(indexSelected(const QModelIndex&)), this, SLOT(indexSelected(const QModelIndex&)));
#endif

     _directoryDock->setWidget(_directoryView);
     addDockWidget(Qt::LeftDockWidgetArea, _directoryDock);;
  _windowMenu->addAction(_directoryDock->toggleViewAction());
}

void MainWindow::updateActions()
{
  bool isEnabled = !_imageFilename.isEmpty() && !_fitToWindowAction->isChecked();

  _fitToWindowAction  ->setEnabled(!_imageFilename.isEmpty());
  _zoomInAction       ->setEnabled(isEnabled);
  _zoomOutAction      ->setEnabled(isEnabled);
  _setNormalSizeAction->setEnabled(isEnabled);
  _setFullSizeAction  ->setEnabled(isEnabled);
}

void MainWindow::setTitle()
{
  QString title = "jpgmagick";

  if (!_imageFilename.isEmpty())
  {
    title.append(QString(" - %1 (%2%)").arg(_imageFilename).arg((int) (_scaleFactor * 100.0)));
  }

  setWindowTitle(title);
}

// == Image ===================================================================
void MainWindow::setImage1(const QString &filename)
{
  if (openImage2(filename, _image1))
  {
    drawImages(_imageLabel, _image1, _image2);

    _imageFilename = filename;

    _scaleFactor = 1.0;


    if (!_fitToWindowAction->isChecked())
    {
      setNormalSize();
    }
  }
  else
  {
    _imageFilename.clear();
  }

  setTitle();

  updateActions();

  clearImage2();
}

void MainWindow::clearImage1()
{
  _image1 = QImage();

  _imageLabel->setPixmap(QPixmap());

  _imageFilename.clear();

  clearImage2();

  setTitle();

  updateActions();
}

void MainWindow::clearImage2()
{
  _image2 = QImage();

  drawImages(_imageLabel, _image1, _image2);

  _saveAction->setEnabled(false);
}

bool MainWindow::openImage(const QString &filename, QLabel *label, QImage &image)
{
  QImageReader reader(filename);

#if QT_VERSION >= 0x050500
  reader.setAutoTransform(true);
#endif

  image = reader.read();

  if (image.isNull())
  {
    QMessageBox::warning(
          this,
          tr("Error: unable to load image"),
          tr("Unable to load the image %1: %2").arg(QDir::toNativeSeparators(filename), reader.errorString()));
  }
  else
  {
    label->setPixmap(QPixmap::fromImage(image));
  }

  return (!image.isNull());
}

bool MainWindow::openImage2(const QString &filename, QImage &image)
{
  QImageReader reader(filename);

#if QT_VERSION >= 0x050500
  reader.setAutoTransform(true);
#endif

  image = reader.read();

  if (image.isNull())
  {
    QMessageBox::warning(
          this,
          tr("Error: unable to load image"),
          tr("Unable to load the image %1: %2").arg(QDir::toNativeSeparators(filename), reader.errorString()));
  }

  return (!image.isNull());
}

void MainWindow::drawImages(QLabel *label, const QImage &image1, const QImage &image2)
{
  QPixmap pixmap(image1.size());

  {
    QPainter painter(&pixmap);

    if (!image1.isNull())
    {
      if (image2.isNull())
      {
        painter.drawImage(0, 0, image1);
      }
      else
      {
        int w = image1.size().width();
        int h = image2.size().height();

        painter.drawImage(QPoint(0,0), image1, QRect(0, 0, w/2, h));
        painter.drawImage(QPoint(w/2,0), image2, QRect(w/2, 0, w/2, h));
      }
    }
  }

  label->setPixmap(pixmap);
}

void MainWindow::scaleImages(double factor)
{
  _scaleFactor *= factor;

  sizeImages();

  adjustScrollBar(_imageScrollArea->horizontalScrollBar(), factor);
  adjustScrollBar(_imageScrollArea->verticalScrollBar(),   factor);

  _zoomInAction ->setEnabled(_scaleFactor < 2.0);
  _zoomOutAction->setEnabled(_scaleFactor > 0.1);

  setTitle();
}

void MainWindow::sizeImages()
{
  _imageLabel->resize(_scaleFactor * _imageLabel->pixmap()->size());
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
  scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

// == Slots ===================================================================

void MainWindow::openDirectory()
{
  QFileDialog directoryDialog(this, tr("Open image directory"), _imagePath);

  directoryDialog.setAcceptMode(QFileDialog::AcceptOpen);
  directoryDialog.setFileMode  (QFileDialog::Directory);
  directoryDialog.setOption    (QFileDialog::ShowDirsOnly);

  if (directoryDialog.exec() == QDialog::Accepted)
  {
    deselectDirectorySelections();

    _imagePath = directoryDialog.selectedFiles().first();

    _directoryView->setRootIndex(_fileSystemModel->setRootPath(_imagePath));
  }

  directoryDialog.close();
}

void MainWindow::saveConverted()
{
  QFileDialog saveDialog(this, tr("Save converted image"), _imageFilename);

  saveDialog.setAcceptMode(QFileDialog::AcceptSave);
  saveDialog.setFileMode  (QFileDialog::AnyFile);

  if (saveDialog.exec() == QDialog::Accepted)
  {
    QString newFilename = saveDialog.selectedFiles().first();

    if (QFile::exists(newFilename))
    {
      QFile::remove(newFilename);
    }

    if (!QFile::copy(_tempFilename, newFilename))
    {
      QMessageBox::critical(this, tr("Error"), tr("Unable to copy %1 to %2").arg(_tempFilename, newFilename));
    }

    setImage1(_imageFilename);
  }

  saveDialog.close();
}

void MainWindow::parentDirectory()
{
  QDir directory = _fileSystemModel->rootDirectory();

  if (!directory.isRoot())
  {
    directory.cdUp();

    deselectDirectorySelections();

    _imagePath = directory.absolutePath();

    clearImage1();

    _directoryView->setRootIndex(_fileSystemModel->setRootPath(_imagePath));
  }
}

void MainWindow::zoomIn()
{
  scaleImages(1.25);
}

void MainWindow::zoomOut()
{
  scaleImages(0.75);
}

void MainWindow::setNormalSize()
{
  _scaleFactor = 1.0;

  // Scale the image to fit on screen
  double factor = qMin(double(_imageScrollArea->size().width())  / double(_image1.size().width()),
                       double(_imageScrollArea->size().height()) / double(_image1.size().height()));

  scaleImages(factor);
}

void MainWindow::setFullSize()
{
  _scaleFactor = 1.0;

  _imageLabel->adjustSize();
}

void MainWindow::fitToWindow()
{
  bool fitToWindow = _fitToWindowAction->isChecked();

  _imageScrollArea->setWidgetResizable(fitToWindow);

  if (!fitToWindow)
  {
     setNormalSize();
  }

  updateActions();
}

void MainWindow::directoryLoaded(const QString &)
{
#if 1
  // Strange qt bug: directoryLoaded event is too early -> indices are not yet correct
  QTime waitTime = QTime::currentTime().addMSecs( 200 );
  while (QTime::currentTime() < waitTime)
  {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  }
#endif

  if (_imageFilename.isEmpty())
  {
    moveToFirstImage();
  }
  else
  {
    moveToCurrentImage();
  }
}

void MainWindow::rootPathChanged(const QString &path)
{
  _directoryDock->setWindowTitle(tr("Images: %1").arg(path));
}


void MainWindow::moveToIndex(const QModelIndex &index)
{
  if (index != QModelIndex())
  {
    deselectDirectorySelections();

    QString filename = _fileSystemModel->fileInfo(index).absoluteFilePath();

    if (_fileSystemModel->fileInfo(index).isDir())
    {
      clearImage1();

      _directoryView->setRootIndex(_fileSystemModel->setRootPath(filename));
    }
    else
    {
      _directoryView->selectionModel()->select(index, QItemSelectionModel::Select);
      _directoryView->scrollTo(index);

      setImage1(filename);
    }
  }
}

void MainWindow::indexActivated(const QModelIndex &index)
{
  if (index != QModelIndex())
  {
    QString filename = _fileSystemModel->fileInfo(index).absoluteFilePath();

    if (_fileSystemModel->fileInfo(index).isDir())
    {
      clearImage1();

      _directoryView->setRootIndex(_fileSystemModel->setRootPath(filename));
    }
    else if (filename != _imageFilename)
    {
      setImage1(filename);
    }
  }
}

void MainWindow::indexSelected(const QModelIndex &index)
{
  if (index != QModelIndex())
  {
    QString filename = _fileSystemModel->fileInfo(index).absoluteFilePath();

    if (_fileSystemModel->fileInfo(index).isDir())
    {
      clearImage1();
    }
    else if (filename != _imageFilename)
    {
      setImage1(filename);
    }
  }
}

void MainWindow::moveToPrevImage()
{
  QModelIndexList selectedIndexes =  _directoryView->selectionModel()->selectedIndexes();

  if (selectedIndexes.size() >= 1)
  {
    QModelIndex selected = selectedIndexes.first();

    int row = selected.row();
    int col = selected.column();

    QModelIndex sibling;

    do
    {
       sibling = selected.sibling(--row, col);
    }
    while ((sibling != QModelIndex()) && (_fileSystemModel->isDir(sibling)));

    moveToIndex(sibling);
  }
  else
  {
    QMessageBox::warning(this, tr("Warning"), tr("No selected image"));
  }
}

void MainWindow::moveToNextImage()
{
  QModelIndexList selectedIndexes =  _directoryView->selectionModel()->selectedIndexes();

  if (selectedIndexes.size() >= 1)
  {
    QModelIndex selected = selectedIndexes.first();

    int row = selected.row();
    int col = selected.column();

    QModelIndex sibling;

    do
    {
      sibling = selected.sibling(++row, col);
    }
    while ((sibling != QModelIndex()) && (_fileSystemModel->isDir(sibling)));

    moveToIndex(sibling);
  }
  else
  {
    QMessageBox::warning(this, tr("Warning"), tr("No selected image"));
  }
}

void MainWindow::doConvertImage()
{
  int contrast;
  int brightness;

  _contrastBrightnessTab->get(contrast, brightness);

  if (contrast == 0 && brightness == 0)
  {
    clearImage2();
  }
  else
  {
    _contrastBrightnessTab->setDisabled(true);

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QStringList params;

    params << "-brightness-contrast" << QString("%1x%2").arg(brightness).arg(contrast) << _imageFilename << _tempFilename;

    _magick.convert(params);
  }
}

void MainWindow::imageConverted()
{
  if (openImage2(_tempFilename, _image2))
  {
    drawImages(_imageLabel, _image1, _image2);

    sizeImages();

    _saveAction->setEnabled(true);
  }
  else
  {
    clearImage2();
  }

  QApplication::restoreOverrideCursor();

  _contrastBrightnessTab->setDisabled(false);
}


void MainWindow::about()
{
  QMessageBox::about(this, tr("About jpgmagick"),
          tr("jpgmagick convert images with ImageMagick.\n\n"
             "jpgmagick Copyright (C) 2017\n\n"
             "This program comes with ABSOLUTELY NO WARRANTY;\n"
             "This is free software, and you are welcome to redistribute it "
             "under certain conditions; see the license for details."));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  QFile::remove(_tempFilename);

  saveSettings();

  QMainWindow::closeEvent(event);
}

// == Settings ================================================================
void MainWindow::saveSettings()
{
  QSettings settings;

  settings.setValue("MainWindow/Geometry", saveGeometry());
  settings.setValue("MainWindow/State",    saveState()   );
}

void MainWindow::restoreSettings()
{
  QSettings settings;

  restoreGeometry(settings.value("MainWindow/Geometry").toByteArray());
  restoreState   (settings.value("MainWindow/State"   ).toByteArray());
}

// == Privates ================================================================

void MainWindow::deselectDirectorySelections()
{
  QModelIndexList indexes =  _directoryView->selectionModel()->selectedIndexes();

  for (QModelIndexList::iterator index = indexes.begin(); index != indexes.end(); ++index)
  {
    _directoryView->selectionModel()->select(*index, QItemSelectionModel::Deselect);
  }
}

void MainWindow::moveToCurrentImage()
{
  int col = _directoryView->modelColumn();
  int row = 0;

  QModelIndex root = _directoryView->rootIndex();

  QModelIndex sibling = root.child(row, col);

  while (sibling != QModelIndex() && _fileSystemModel->fileInfo(sibling).absoluteFilePath() != _imageFilename)
  {
    sibling = sibling.sibling(++row, col);
  }

  if (sibling != QModelIndex())
  {
    _directoryView->selectionModel()->select(sibling, QItemSelectionModel::Select);
    _directoryView->scrollTo(sibling);
  }
  else
  {
    moveToFirstImage();
  }
}

QModelIndex MainWindow::getFirstIndex()
{
  int col = _directoryView->modelColumn();
  int row = 0;

  QModelIndex root = _directoryView->rootIndex();

  QModelIndex sibling = root.child(row, col);

  while ((sibling != QModelIndex()) && (_fileSystemModel->isDir(sibling)))
  {
    sibling = sibling.sibling(++row, col);
  }

  return sibling;
}

void MainWindow::moveToFirstImage()
{
  moveToIndex(getFirstIndex());
}

// ============================================================================

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "MainWindow.h"

MainWindow::MainWindow(int argc, char *argv[]) :
  _imageFilename(""),
  _imagePath(QDir::currentPath()),
  _scaleFactor(1.0)
{
  QCoreApplication::setOrganizationName("jpgmagick");
  QCoreApplication::setApplicationName("jpgmagick");

  if (argc > 1)
  {
    if (QFileInfo(argv[1]).isDir())
    {
      _imagePath = argv[1];
    }
  }

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

    _imageScrollArea = new QScrollArea;
      _imageScrollArea->setBackgroundRole(QPalette::Base);
      _imageScrollArea->setWidget(_imageLabel);
      _imageScrollArea->setVisible(true);
      _imageScrollArea->setAlignment(Qt::AlignCenter);

    vbox->addWidget(_imageScrollArea);

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
  connect(_openDirectoryAction, SIGNAL(triggered()), this, SLOT(openDirectory()));

  QIcon parentDirectoryIcon = style->standardIcon(QStyle::SP_FileDialogToParent);
  _parentDirectoryAction = new QAction(parentDirectoryIcon, tr("&Goto Parent Directory"), this);
  _parentDirectoryAction->setStatusTip(tr("Goto the parent directory"));
  connect(_parentDirectoryAction, SIGNAL(triggered()), this, SLOT(parentDirectory()));

  _quitAction = new QAction(tr("&Quit"), this);
  _quitAction->setShortcuts(QKeySequence::Quit);
  _quitAction->setStatusTip(tr("Quit the application"));
  connect(_quitAction, SIGNAL(triggered()), this, SLOT(close()));

  _zoomInAction = new QAction(tr("Zoom &In (25%)"), this);
  _zoomInAction->setShortcut(QKeySequence::ZoomIn);
  _zoomInAction->setEnabled(false);
  connect(_zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

  _zoomOutAction = new QAction(tr("Zoom &Out (25%)"), this);
  _zoomOutAction->setShortcut(QKeySequence::ZoomOut);
  _zoomOutAction->setEnabled(false);
  connect(_zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

  _setNormalSizeAction = new QAction(tr("&Normal Size"), this);
  _setNormalSizeAction->setShortcut(tr("Ctrl+S"));
  _setNormalSizeAction->setEnabled(false);
  connect(_setNormalSizeAction, SIGNAL(triggered()), this, SLOT(setNormalSize()));

  _setFullSizeAction = new QAction(tr("&Full Size"), this);
  _setFullSizeAction->setShortcut(tr("Ctrl+F"));
  _setFullSizeAction->setEnabled(false);
  connect(_setFullSizeAction, SIGNAL(triggered()), this, SLOT(setFullSize()));

  _fitToWindowAction = new QAction(tr("&Fit to Window"), this);
  _fitToWindowAction->setCheckable(true);
  _fitToWindowAction->setShortcut(tr("Ctrl+W"));
  _fitToWindowAction->setEnabled(false);
  connect(_fitToWindowAction, SIGNAL(triggered()), this, SLOT(fitToWindow()));

  QIcon prevImageIcon = style->standardIcon(QStyle::SP_ArrowBack);
  _prevImageAction = new QAction(prevImageIcon, tr("Show prevous image"), this);
  connect(_prevImageAction, SIGNAL(triggered()), this, SLOT(selectPrevImage()));

  QIcon nextImageIcon = style->standardIcon(QStyle::SP_ArrowForward);
  _nextImageAction = new QAction(nextImageIcon, tr("Show next image"), this);
  connect(_nextImageAction, SIGNAL(triggered()), this, SLOT(selectNextImage()));

  _aboutAction = new QAction(tr("&About"), this);
  _aboutAction->setStatusTip(tr("Show the application's About box"));
  connect(_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

  _aboutQtAction = new QAction(tr("About &Qt"), this);
  _aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
  connect(_aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
  _fileMenu = menuBar()->addMenu(tr("&File"));
  _fileMenu->addAction(_openDirectoryAction);
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
     connect(_fileSystemModel, SIGNAL(directoryLoaded(const QString&)), this, SLOT(directoryLoaded(const QString&)));
     connect(_fileSystemModel, SIGNAL(rootPathChanged(const QString&)), this, SLOT(rootPathChanged(const QString&)));

     _directoryView = new QListView(_directoryDock);
    _directoryView->setModel(_fileSystemModel);
    _directoryView->setRootIndex(_fileSystemModel->index(_imagePath));
    connect(_directoryView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(selectInDirectory(const QModelIndex&)));

    _directoryDock->setWidget(_directoryView);
    addDockWidget(Qt::LeftDockWidgetArea, _directoryDock);;
  _windowMenu->addAction(_directoryDock->toggleViewAction());
}

void MainWindow::updateActions()
{
  _zoomInAction       ->setEnabled(!_fitToWindowAction->isChecked());
  _zoomOutAction      ->setEnabled(!_fitToWindowAction->isChecked());
  _setNormalSizeAction->setEnabled(!_fitToWindowAction->isChecked());
  _setFullSizeAction  ->setEnabled(!_fitToWindowAction->isChecked());
}

void MainWindow::setTitle()
{
  QString title = "jpgmagick";

  if (_imageFilename.length() > 0)
  {
    title.append(QString(" - %1 (%2%)").arg(_imageFilename).arg((int) (_scaleFactor * 100.0)));
  }

  setWindowTitle(title);
}

// == Image ===================================================================
void MainWindow::openImage(const QString &filename)
{
  QImageReader reader(filename);

#if QT_VERSION >= 0x050500
  reader.setAutoTransform(true);
#endif

  const QImage image = reader.read();

  if (image.isNull())
  {
    QMessageBox::information(this,
                             tr("Error: unable to load image"),
                             tr("Unable to load the image %1: %2").arg(QDir::toNativeSeparators(filename), reader.errorString()));
  }
  else
  {
    _imageFilename = filename;

    setTitle();

    setImage(image);
  }
}

void MainWindow::setImage(const QImage &image)
{
  _image = image;

  _imageLabel->setPixmap(QPixmap::fromImage(_image));

  _scaleFactor = 1.0;

  _fitToWindowAction->setEnabled(true);

  updateActions();

  if (!_fitToWindowAction->isChecked())
  {
    setNormalSize();
  }
}

void MainWindow::scaleImage(double factor)
{
  _scaleFactor *= factor;

  _imageLabel->resize(_scaleFactor * _imageLabel->pixmap()->size());

  adjustScrollBar(_imageScrollArea->horizontalScrollBar(), factor);
  adjustScrollBar(_imageScrollArea->verticalScrollBar(),   factor);

  _zoomInAction ->setEnabled(_scaleFactor < 2.0);
  _zoomOutAction->setEnabled(_scaleFactor > 0.1);

  setTitle();
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

    _directoryDock->setWindowTitle(_imagePath);

    _fileSystemModel->setRootPath(_imagePath);

    _directoryView->setRootIndex(_fileSystemModel->setRootPath(_imagePath));
  }

  directoryDialog.close();
}

void MainWindow::parentDirectory()
{
  QDir directory = _fileSystemModel->rootDirectory();

  if (!directory.isRoot())
  {
    directory.cdUp();

    deselectDirectorySelections();

    _imagePath = directory.absolutePath();
    _imageFilename = "";
    _imageLabel->setPixmap(QPixmap());

    _directoryDock->setWindowTitle(tr("Images: %1").arg(_imagePath));

    _fileSystemModel->setRootPath(_imagePath);

    _directoryView->setRootIndex(_fileSystemModel->setRootPath(_imagePath));
  }
}

void MainWindow::zoomIn()
{
  scaleImage(1.25);
}

void MainWindow::zoomOut()
{
  scaleImage(0.75);
}

void MainWindow::setNormalSize()
{
  _scaleFactor = 1.0;

  // Scale the image to fit on screen
  double factor = qMin((double) _imageScrollArea->size().width()  / (double) _image.size().width(),
                       (double) _imageScrollArea->size().height() / (double) _image.size().height());

  scaleImage(factor * 0.95);
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

  if (_imageFilename == "")
  {
    selectFirstImage();
  }
  else
  {
    selectCurrentImage();
  }
}

void MainWindow::rootPathChanged(const QString &)
{
}


void MainWindow::selectInDirectory(const QModelIndex &index)
{
  if (index != QModelIndex())
  {
    deselectDirectorySelections();

    QString filename = _fileSystemModel->fileInfo(index).absoluteFilePath();

    if (_fileSystemModel->fileInfo(index).isDir())
    {
      _imageFilename = "";
      _imageLabel->setPixmap(QPixmap());

      _directoryDock->setWindowTitle(filename);

      _fileSystemModel->setRootPath(filename);

      _directoryView->setRootIndex(_fileSystemModel->setRootPath(filename));
    }
    else
    {
      _directoryView->selectionModel()->select(index, QItemSelectionModel::Select);
      _directoryView->scrollTo(index);

      openImage(filename);
    }
  }
}

void MainWindow::selectPrevImage()
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

    selectInDirectory(sibling);
  }
  else
  {
    QMessageBox::warning(0, tr("Warning"), tr("No selected image"));
  }
}

void MainWindow::selectNextImage()
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

    selectInDirectory(sibling);
  }
  else
  {
    QMessageBox::warning(0, tr("Warning"), tr("No selected image"));
  }
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("About jpgmagick"),
          tr("jpgmagick manipulates the exif metadata in images.\n\n"
             "jpgmagick Copyright (C) 2017\n\n"
             "This program comes with ABSOLUTELY NO WARRANTY;\n"
             "This is free software, and you are welcome to redistribute it "
             "under certain conditions; see the license for details."));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
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

void MainWindow::selectCurrentImage()
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
    selectFirstImage();
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

void MainWindow::selectFirstImage()
{
  selectInDirectory(getFirstIndex());
}

// ============================================================================

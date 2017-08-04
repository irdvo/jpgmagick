VERSION = 0.1

TEMPLATE = app
CONFIG  += qt
equals(QT_MAJOR_VERSION,5) {
  QT      += widgets
}
SOURCES  += main.cpp\
            MainWindow.cpp\
            ContrastBrightnessTab.cpp
            
HEADERS  += MainWindow.h\
            ContrastBrightnessTab.h

#RESOURCES = jpgmagick.qrc
TARGET    = jpgmagick

message(Includepath: $$INCLUDEPATH)
message(DestDir:     $$DESTDIR)
message(Libs:        $$LIBS)
message(Qt version:  $$[QT_VERSION])


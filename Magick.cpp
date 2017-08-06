#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "Magick.h"


Magick::Magick(QObject *parent) :
  QProcess(parent)
{
  connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
  connect(this, SIGNAL(readyReadStandardError()),            this, SLOT(standardError()));
  connect(this, SIGNAL(readyReadStandardOutput()),           this, SLOT(standardOut()));
}

Magick::~Magick()
{

}

// -- Update the exif metadata for the image -----------------------------------
bool Magick::convert(const QStringList &params)
{
  if (state() != QProcess::NotRunning)
  {
    return false;
  }

  start("convert", params);

  return true;
}


// -- Slots -------------------------------------------------------------------
void Magick::standardError()
{
  QTextStream out(stderr);

  out << QString(readAllStandardError());
}

void Magick::standardOut()
{
  QTextStream out(stdout);

  out << QString(readAllStandardError());
}

void Magick::finished(int, ExitStatus)
{
  emit converted();
}

// ----------------------------------------------------------------------------

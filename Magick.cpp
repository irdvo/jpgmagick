#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "Magick.h"

void finished(int exitCode, QProcess::ExitStatus exitStatus);

Magick::Magick(QObject *parent) :
  QProcess(parent)
{
#if QT_VERSION >= 0x050000
  connect(this, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
          this, static_cast<void (Magick  ::*)(int, QProcess::ExitStatus)>(&Magick  ::finished));
  connect(this, &QProcess::readyReadStandardError,  this, &Magick::standardError);
  connect(this, &QProcess::readyReadStandardOutput, this, &Magick::standardOut);
#else
  connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
  connect(this, SIGNAL(readyReadStandardError()),            this, SLOT(standardError()));
  connect(this, SIGNAL(readyReadStandardOutput()),           this, SLOT(standardOut()));
#endif
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

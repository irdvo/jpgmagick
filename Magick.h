#ifndef MAGICK_H
#define MAGICK_H

#include <QProcess>

class Magick : public QProcess
{
  Q_OBJECT

public:
  Magick(QObject *parent = 0);
  ~Magick();

  // -- Commands --------------------------------------------------------------
  bool convert(const QStringList &params);

private slots:
  void standardError();
  void standardOut();
  void finished(int exitCode, QProcess::ExitStatus exitStatus);

signals:
  void converted();
};

#endif

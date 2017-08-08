#ifndef CONTRASTBRIGHTNESSTAB_H
#define CONTRASTBRIGHTNESSTAB_H

#include <QWidget>

class QSpinBox;
class QPushButton;

class ContrastBrightnessTab : public QWidget
{
  Q_OBJECT

public:
  ContrastBrightnessTab(QWidget *parent = 0);
  
  void reset();

  void get(int &contrast, int &brightness);

signals:
  void doConvert();
  void nextImage();
  
private slots:
  void convertClicked();
  void nextClicked();

private:
  QSpinBox       *_contrastBox;
  QSpinBox       *_brightnessBox;
  QPushButton    *_convertButton;
  QPushButton    *_nextButton;
};

#endif // CONTRASTBRIGHTNESSTAB_H

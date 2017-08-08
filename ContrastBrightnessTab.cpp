#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>

#include "ContrastBrightnessTab.h"


ContrastBrightnessTab::ContrastBrightnessTab(QWidget *parent) :
  QWidget(parent)
{
  QHBoxLayout *layout = new QHBoxLayout;

  layout->addWidget(new QLabel(tr("Contrast:")));

  _contrastBox = new QSpinBox;
    _contrastBox->setMinimum(-20);
    _contrastBox->setMaximum(20);
    _contrastBox->setSingleStep(5);
    layout->addWidget(_contrastBox);

  layout->addWidget(new QLabel(tr("Brightness:")));

  _brightnessBox = new QSpinBox;
    _brightnessBox->setMinimum(-20);
    _brightnessBox->setMaximum(20);
    _brightnessBox->setSingleStep(5);
    layout->addWidget(_brightnessBox);

  _convertButton = new QPushButton(tr("Convert"));
#if QT_VERSION >= 0x050000
    connect(_convertButton, &QPushButton::clicked, this, &ContrastBrightnessTab::convertClicked);
#else
    connect(_convertButton, SIGNAL(clicked()), this, SLOT(convertClicked()));
#endif
    layout->addWidget(_convertButton);

    _nextButton = new QPushButton(tr(">>"));
#if QT_VERSION >= 0x050000
    connect(_nextButton, &QPushButton::clicked, this, &ContrastBrightnessTab::nextClicked);
#else
    connect(_nextButton, SIGNAL(clicked()), this, SLOT(nextClicked()));
#endif
    layout->addWidget(_nextButton);

  layout->addStretch(1);

  setLayout(layout);
}

void ContrastBrightnessTab::reset()
{
  _contrastBox->setValue(0);
  _brightnessBox->setValue(0);
}

void ContrastBrightnessTab::get(int &contrast, int &brightness)
{
  contrast = _contrastBox->value();
  brightness = _brightnessBox->value();
}

void ContrastBrightnessTab::convertClicked()
{
  emit doConvert();
}

void ContrastBrightnessTab::nextClicked()
{
  emit nextImage();
}

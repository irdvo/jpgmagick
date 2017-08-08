#ifndef FILEVIEW_H
#define FILEVIEW_H

#include <QListView>

class FileView : public QListView
{
  Q_OBJECT

public:
  FileView(QWidget *parent = 0);
  
signals:
  
protected slots:
  void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
};

#endif

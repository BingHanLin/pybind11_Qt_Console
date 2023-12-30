#pragma once

#include <QMainWindow>
#include <QObject>

class mainWindow : public QMainWindow {
  Q_OBJECT

public:
  mainWindow(QWidget *parent = nullptr);
};

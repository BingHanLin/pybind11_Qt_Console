#include <QApplication>
#include <QMainWindow>

#include "mainWindow.hpp"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  mainWindow w;
  w.show();
  return a.exec();
}
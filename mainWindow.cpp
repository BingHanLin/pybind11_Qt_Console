
#include "mainWindow.hpp"
#include "pythonTerminal.hpp"

mainWindow::mainWindow(QWidget *parent) : QMainWindow(parent) {

  this->setCentralWidget(new pythonTerminalEdit(this));
}

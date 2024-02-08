#include <QApplication>
#include <QMainWindow>

#include "mainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    mainWindow window;
    window.showNormal();

    return QApplication::exec();
}
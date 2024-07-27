#include <QApplication>
#include <QMainWindow>

#include "mainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    mainWindow window;
    window.resize(800, 600);
    window.show();

    return QApplication::exec();
}
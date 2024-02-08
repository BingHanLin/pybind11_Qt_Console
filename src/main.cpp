#include <QApplication>
#include <QMainWindow>

#include "mainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    mainWindow window;
    window.resize(400, 300);
    window.show();

    return QApplication::exec();
}
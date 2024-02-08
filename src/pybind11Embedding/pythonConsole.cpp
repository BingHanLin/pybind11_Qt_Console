#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStringListModel>
#include <QTextEdit>
#include <qboxlayout.h>

#include "pythonCommandLine.hpp"
#include "pythonConsole.hpp"

pythonConsole::pythonConsole(QWidget* parent) : QWidget(parent), outputTextEdit_(nullptr)
{
    outputTextEdit_ = new QTextEdit(this);
    outputTextEdit_->setReadOnly(true);

    auto commandLine = new pythonCommandLine(this);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(outputTextEdit_);
    layout->addWidget(commandLine);
    layout->setStretch(0, 1);
    layout->setStretch(1, 0);
}

void pythonConsole::showMessage(const QString& message)
{
}

void pythonConsole::onCommandInserted(const QString& commands)
{
}

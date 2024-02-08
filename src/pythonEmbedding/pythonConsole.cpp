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

    auto interpreter = std::make_shared<pythonInterpreter>();
    auto commandLine = new pythonCommandLine(interpreter, this);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(outputTextEdit_);
    layout->addWidget(commandLine);
    layout->setStretch(0, 1);
    layout->setStretch(1, 0);

    connect(interpreter.get(), &pythonInterpreter::commandInserted, this, &pythonConsole::onCommandInserted);
    connect(interpreter.get(), &pythonInterpreter::commandParsedWithError, this, &pythonConsole::onCommandParseError);
}

void pythonConsole::onCommandInserted(const QString& commands)
{
    outputTextEdit_->insertPlainText(QString(">>>  %1\n").arg(commands));
}

void pythonConsole::onCommandParseError(const QString& message)
{
    outputTextEdit_->insertPlainText(QString("Parse with error:\n%1\n").arg(message));
}

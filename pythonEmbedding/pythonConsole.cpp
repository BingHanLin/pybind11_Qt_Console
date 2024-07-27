#include <QAbstractItemView>
#include <QKeyEvent>
#include <QLabel>
#include <QScrollBar>
#include <QStringListModel>
#include <QTextEdit>
#include <QVBoxLayout>

#include "pythonCommandLine.hpp"
#include "pythonConsole.hpp"

pythonConsole::pythonConsole(QWidget* parent) : QWidget(parent), outputTextEdit_(nullptr)
{
    outputTextEdit_ = new QTextEdit(this);
    outputTextEdit_->setReadOnly(true);

    auto interpreter = std::make_shared<pythonInterpreter>();
    auto commandLine = new pythonCommandLine(interpreter, this);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(9, 9, 9, 9);

    layout->addWidget(outputTextEdit_);
    layout->addWidget(commandLine);
    layout->setStretch(0, 1);
    layout->setStretch(1, 0);

    connect(interpreter.get(), &pythonInterpreter::pyCommandBeforeInserted, this, &pythonConsole::onCommandInserted);
    connect(interpreter.get(), &pythonInterpreter::pyCommandStdOutput, this, &pythonConsole::onCommandStdOutput);
    connect(interpreter.get(), &pythonInterpreter::pyCommandParsedWithError, this, &pythonConsole::onCommandParseError);
}

void pythonConsole::onMessagePassedIn(const QString& message)
{
    outputTextEdit_->insertPlainText(QString("%1\n").arg(message));
}

void pythonConsole::onCommandInserted(const QString& commands)
{
    outputTextEdit_->insertPlainText(QString(">>>  %1\n").arg(commands));
}

void pythonConsole::onCommandStdOutput(const QString& outputs)
{
    outputTextEdit_->insertPlainText(QString("%1\n").arg(outputs));
}

void pythonConsole::onCommandParseError(const QString& message)
{
    outputTextEdit_->insertPlainText(QString("Parse with error:\n%1\n").arg(message));
}

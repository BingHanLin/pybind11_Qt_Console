#include <QAbstractItemView>
#include <QKeyEvent>
#include <QLabel>
#include <QScrollBar>
#include <QStringListModel>
#include <QTextEdit>
#include <QVBoxLayout>

#include "pythonCommandLine.hpp"
#include "pythonConsole.hpp"

pythonConsole::pythonConsole(QWidget* parent) : QWidget(parent), outputTextEdit_(nullptr), commandLine_(nullptr)
{
    outputTextEdit_ = new QTextEdit(this);
    outputTextEdit_->setReadOnly(true);

    auto interpreter = std::make_shared<pythonInterpreter>();
    commandLine_ = new pythonCommandLine(interpreter, this);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(9, 9, 9, 9);

    layout->addWidget(outputTextEdit_);
    layout->addWidget(commandLine_);
    layout->setStretch(0, 1);
    layout->setStretch(1, 0);

    connect(interpreter.get(), &pythonInterpreter::pyCommandBeforeInserted, this, &pythonConsole::onCommandInserted);
    connect(interpreter.get(), &pythonInterpreter::pyCommandStdOutput, this, &pythonConsole::onCommandStdOutput);
    connect(interpreter.get(), &pythonInterpreter::pyCommandParsedWithError, this, &pythonConsole::onCommandParseError);
}

void pythonConsole::setConsoleEnabled(bool enabled)
{
    commandLine_->setEnabled(enabled);
}

void pythonConsole::onMessagePassedIn(const QString& message)
{
    auto test = message.toStdString();

    outputTextEdit_->append(QString("%1\n").arg(message));

    outputTextEdit_->verticalScrollBar()->setValue(outputTextEdit_->verticalScrollBar()->maximum());
}

void pythonConsole::onCommandInserted(const QString& commands)
{
    auto test = commands.toStdString();

    outputTextEdit_->append(QString(">>>  %1\n").arg(commands));

    outputTextEdit_->verticalScrollBar()->setValue(outputTextEdit_->verticalScrollBar()->maximum());
}

void pythonConsole::onCommandStdOutput(const QString& outputs)
{
    auto test = outputs.toStdString();

    outputTextEdit_->append(QString("%1\n").arg(outputs));

    outputTextEdit_->verticalScrollBar()->setValue(outputTextEdit_->verticalScrollBar()->maximum());
}

void pythonConsole::onCommandParseError(const QString& message)
{
    auto test = message.toStdString();

    outputTextEdit_->append(QString("Parse with error:\n%1\n").arg(message));

    outputTextEdit_->verticalScrollBar()->setValue(outputTextEdit_->verticalScrollBar()->maximum());
}

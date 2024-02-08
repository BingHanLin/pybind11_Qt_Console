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
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(new QLabel(tr("Console")));
    layout->addWidget(outputTextEdit_);
    layout->addWidget(commandLine);
    layout->setStretch(0, 1);
    layout->setStretch(1, 0);

    connect(interpreter.get(), &pythonInterpreter::commandInserted, this, &pythonConsole::onCommandInserted);
    connect(interpreter.get(), &pythonInterpreter::commandParsedWithError, this, &pythonConsole::onCommandParseError);
}

void pythonConsole::onMessagePassedIn(const QString& message)
{
    outputTextEdit_->insertPlainText(QString("%1\n").arg(message));
}

void pythonConsole::onCommandInserted(const QString& commands)
{
    outputTextEdit_->insertPlainText(QString(">>>  %1\n").arg(commands));
}

void pythonConsole::onCommandParseError(const QString& message)
{
    outputTextEdit_->insertPlainText(QString("Parse with error:\n%1\n").arg(message));
}

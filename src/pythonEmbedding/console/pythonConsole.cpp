#include <QAbstractItemView>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <QStringListModel>
#include <QTextEdit>
#include <QVBoxLayout>

#include "pythonCommandLine.hpp"
#include "pythonConsole.hpp"

pythonConsole::pythonConsole(QWidget* parent)
    : QWidget(parent), outputTextEdit_(nullptr), interpreter_(nullptr), commandLine_(nullptr)
{
    outputTextEdit_ = new QTextEdit(this);
    outputTextEdit_->setReadOnly(true);

    interpreter_ = std::make_shared<pythonInterpreter>();
    commandLine_ = new pythonCommandLine(interpreter_, this);

    auto buttonWidget = new QWidget(this);
    auto buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    runFromScriptButton_ = new QPushButton("Run Script", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(runFromScriptButton_);
    connect(runFromScriptButton_, &QPushButton::clicked, this, &pythonConsole::onRunFromScriptButtonClicked);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(9, 9, 9, 9);

    layout->addWidget(buttonWidget);
    layout->addWidget(outputTextEdit_);
    layout->addWidget(commandLine_);

    layout->setStretch(0, 1);
    layout->setStretch(1, 0);

    connect(interpreter_.get(), &pythonInterpreter::pyCommandBeforeInserted, this, &pythonConsole::onCommandInserted);
    connect(interpreter_.get(), &pythonInterpreter::pyCommandStdOutput, this, &pythonConsole::onCommandStdOutput);
    connect(interpreter_.get(), &pythonInterpreter::pyCommandParsedWithError, this,
            &pythonConsole::onCommandParseError);
}

void pythonConsole::setConsoleEnabled(bool enabled)
{
    commandLine_->setEnabled(enabled);
    runFromScriptButton_->setEnabled(enabled);
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

void pythonConsole::onRunFromScriptButtonClicked()
{
    bool ok;
    QString text = QInputDialog::getMultiLineText(this, tr("Run Script"), tr("Script:"), QString(), &ok);
    if (ok && !text.isEmpty())
    {
        interpreter_->runCommand(text.toStdString());
    }
}

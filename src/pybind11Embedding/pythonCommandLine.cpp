#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStringListModel>
#include <QTextEdit>

#include "pythonCommandLine.hpp"

pythonCommandLine::pythonCommandLine(QWidget *parent)
    : QLineEdit(parent), interpreter_(std::make_shared<pythonInterpreter>()), completer_(new QCompleter(this))
{
    completer_->setWidget(this);
    completer_->setCaseSensitivity(Qt::CaseSensitive);
    completer_->setCompletionMode(QCompleter::PopupCompletion);
    completer_->setCaseSensitivity(Qt::CaseInsensitive);

    this->updateCompleter();

    connect(completer_, QOverload<const QString &>::of(&QCompleter::activated), this,
            &pythonCommandLine::onCompletionActivated);
}

void pythonCommandLine::onCompletionActivated(const QString &completionText)
{
    if (completer_->widget() != this)
    {
        return;
    }

    const auto extraLength = completionText.length() - completer_->completionPrefix().length();

    this->insert(completionText.right(extraLength));
    this->end(false);
}

void pythonCommandLine::keyPressEvent(QKeyEvent *event)
{
    if (completer_ != nullptr && completer_->popup()->isVisible())
    {
        switch (event->key())
        {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Escape:
            case Qt::Key_Tab:
            case Qt::Key_Backtab:
                event->ignore();
                return;
            default:
                break;
        }
    }

    if (event->key() == Qt::Key_Return)
    {
        const QString text = this->text();
        interpreter_->runCommand(text.toStdString());
        this->clear();
        event->accept();
        return;
    }

    QLineEdit::keyPressEvent(event);

    this->updateCompleter();

    const static QString endOfWord("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-=");

    const QString eventText = event->text();
    const QString completionPrefix = extractLastWord(eventText);

    const bool hasModifier = (event->modifiers() != Qt::NoModifier);

    if (hasModifier || eventText.isEmpty() || completionPrefix.length() < 3
        // || endOfWord.contains(eventText.right(1))
    )
    {
        completer_->popup()->hide();
        event->accept();
        return;
    }

    if (completionPrefix != completer_->completionPrefix())
    {
        completer_->setCompletionPrefix(completionPrefix);
        completer_->popup()->setCurrentIndex(completer_->completionModel()->index(0, 0));
    }

    QRect cr = cursorRect();
    cr.setWidth(completer_->popup()->sizeHintForColumn(0) +
                completer_->popup()->verticalScrollBar()->sizeHint().width());
    completer_->complete(cr);
}

void pythonCommandLine::focusInEvent(QFocusEvent *e)
{
    if (completer_ != nullptr)
    {
        completer_->setWidget(this);
    }

    QLineEdit::focusInEvent(e);
}

void pythonCommandLine::updateCompleter()
{
    auto wordStringList = this->text().split(".");
    wordStringList.removeAll("");

    std::vector<std::string> stdWordVector;
    for (const auto &qString : wordStringList)
    {
        stdWordVector.push_back(qString.toStdString());
    }

    const auto stdPossibleWordVector = interpreter_->getPossibleMethods(stdWordVector);

    QStringList possibleWordStringList;
    for (const auto &stdString : stdPossibleWordVector)
    {
        possibleWordStringList.append(QString::fromStdString(stdString));
    }

    auto model = new QStringListModel(possibleWordStringList, completer_);
    completer_->setModel(model);
}

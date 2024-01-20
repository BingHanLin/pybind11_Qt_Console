#include <iostream>

#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStringListModel>

#include "pythonTerminal.hpp"

pythonTerminal::pythonTerminal(std::shared_ptr<pythonInterpreter> interpreter,
                               QWidget *parent)
    : QTextEdit(parent), interpreter_(interpreter), completer_(nullptr)
{
    completer_ = new QCompleter(this);
    completer_->setWidget(this);
    completer_->setCaseSensitivity(Qt::CaseSensitive);
    completer_->setCompletionMode(QCompleter::PopupCompletion);
    completer_->setCaseSensitivity(Qt::CaseInsensitive);

    this->updateCompleter();

    connect(completer_, QOverload<const QString &>::of(&QCompleter::activated),
            this, &pythonTerminal::onCompletionActivated);
}

void pythonTerminal::onCompletionActivated(const QString &completionText)
{
    if (completer_->widget() != this)
    {
        return;
    }

    const int extra =
        completionText.length() - completer_->completionPrefix().length();

    QTextCursor tc = this->textCursor();
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completionText.right(extra));
    this->setTextCursor(tc);
}

void pythonTerminal::keyPressEvent(QKeyEvent *event)
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
        const QString text = this->toPlainText();
        interpreter_->runCommand(text.toStdString());
        event->accept();
        return;
    }

    QTextEdit::keyPressEvent(event);

    this->updateCompleter();

    const static QString endOfWord("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-=");

    const QString eventText = event->text();
    const QString completionPrefix = this->textUnderCursor();

    const bool hasModifier = (event->modifiers() != Qt::NoModifier);

    {
        if (hasModifier || eventText.isEmpty() ||
            completionPrefix.length() < 3 ||
            endOfWord.contains(eventText.right(1)))
        {
            completer_->popup()->hide();
            event->accept();
            return;
        }
    }

    {
        if (completionPrefix != completer_->completionPrefix())
        {
            completer_->setCompletionPrefix(completionPrefix);
            completer_->popup()->setCurrentIndex(
                completer_->completionModel()->index(0, 0));
        }

        QRect cr = cursorRect();
        cr.setWidth(
            completer_->popup()->sizeHintForColumn(0) +
            completer_->popup()->verticalScrollBar()->sizeHint().width());
        completer_->complete(cr);
    }
}

void pythonTerminal::focusInEvent(QFocusEvent *e)
{
    if (completer_ != nullptr)
    {
        completer_->setWidget(this);
    }

    QTextEdit::focusInEvent(e);
}

QString pythonTerminal::textUnderCursor() const
{
    QTextCursor tc = this->textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void pythonTerminal::updateCompleter()
{
    QTextCursor tc = this->textCursor();
    tc.select(QTextCursor::LineUnderCursor);
    const auto lineText = tc.selectedText();
    auto wordStringList = lineText.split(".");
    wordStringList.removeAll("");

    std::vector<std::string> stdWordVector;
    for (const auto &qString : wordStringList)
    {
        stdWordVector.push_back(qString.toStdString());
    }

    const auto stdPossibleWordVector =
        interpreter_->getPossibleMethods(stdWordVector);

    QStringList possibleWordStringList;
    for (const auto &stdString : stdPossibleWordVector)
    {
        // Convert std::string to QString and append it to the QStringList
        possibleWordStringList.append(QString::fromStdString(stdString));
    }

    auto model = new QStringListModel(possibleWordStringList, completer_);
    completer_->setModel(model);
}

#include <iostream>

#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>

#pragma push_macro( \
    "slots")  // solve slots variable name conflits between python and qt.
#undef slots
#include <pybind11/embed.h>  // everything needed for embedding
#pragma pop_macro("slots")

#include "pythonTerminal.hpp"

pythonTerminal::pythonTerminal(std::shared_ptr<pythonInterpreter> interpreter,
                               QWidget *parent)
    : QTextEdit(parent), interpreter_(interpreter), completer_(nullptr)
{
    QStringList wordList;
    wordList << "XiaoTu"
             << "xiaomi"
             << "Huawei"
             << "huafei"
             << "Shanghai"
             << "shangshan"
             << "abc";

    completer_ = new QCompleter(wordList, this);
    // completer->setCompletionMode(QCompleter::InlineCompletion);
    // completer->setCaseSensitivity(Qt::CaseSensitive);

    completer_->setWidget(this);
    completer_->setCompletionMode(QCompleter::PopupCompletion);
    completer_->setCaseSensitivity(Qt::CaseInsensitive);
    connect(completer_, QOverload<const QString &>::of(&QCompleter::activated),
            this, &pythonTerminal::onCompletionActivated);
}

QString pythonTerminal::textUnderCursor() const
{
    QTextCursor tc = this->textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
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
        const QString text = toPlainText();
        interpreter_->runCommand(text.toStdString());
        event->accept();
        return;
    }
    else
    {
        QTextEdit::keyPressEvent(event);

        const static QString endOfWord("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-=");

        const bool hasModifier = (event->modifiers() != Qt::NoModifier);
        const QString completionPrefix = this->textUnderCursor();
        const QString eventText = event->text();

        if (hasModifier || eventText.isEmpty() ||
            completionPrefix.length() < 3 ||
            endOfWord.contains(eventText.right(1)))
        {
            completer_->popup()->hide();
            event->accept();
            return;
        }

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

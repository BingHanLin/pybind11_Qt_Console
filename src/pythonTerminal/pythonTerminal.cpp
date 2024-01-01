

#include <QKeyEvent>

#include "pythonTerminal.hpp"

pythonTerminal::pythonTerminal(std::shared_ptr<pythonInterpreter> interpreter,
                               QWidget *parent)
    : QTextEdit(parent), interpreter_(interpreter)
{
}

void pythonTerminal::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return)
    {
        const QString text = toPlainText();
        interpreter_->runCommand(text.toStdString());
        // interpreter_->runCommand("order_commands.add_order(5, 39.5)");
        event->accept();
        return;
    }

    QTextEdit::keyPressEvent(event);
}

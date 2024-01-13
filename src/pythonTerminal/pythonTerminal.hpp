#pragma once

#include <QCompleter>
#include <QKeyEvent>
#include <QTextEdit>
#include <QWidget>

#include <pythonInterpreter.hpp>

class pythonTerminal : public QTextEdit
{
    Q_OBJECT

   public:
    pythonTerminal(std::shared_ptr<pythonInterpreter> interpreter,
                   QWidget *parent = nullptr);

   protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *e) override;

   private:
    std::shared_ptr<pythonInterpreter> interpreter_;
    QCompleter *completer_;

    QString textUnderCursor() const;

   private slots:
    void onCompletionActivated(const QString &completion);
};

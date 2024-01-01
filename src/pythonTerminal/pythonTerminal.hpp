#pragma once

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

   private:
    std::shared_ptr<pythonInterpreter> interpreter_;
};

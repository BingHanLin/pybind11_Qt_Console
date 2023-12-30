#pragma once

#include <QKeyEvent>
#include <QTextEdit>
#include <QWidget>

#pragma push_macro(                                                            \
    "slots") // solve slots variable name conflits between python and qt.
#undef slots
#include <pybind11/embed.h> // everything needed for embedding
#pragma pop_macro("slots")

class pythonTerminalEdit : public QTextEdit {
  Q_OBJECT

public:
  pythonTerminalEdit(QWidget *parent = 0);

protected:
  void keyPressEvent(QKeyEvent *event) override;

private slots:
  void runCommand();

private:
  pybind11::scoped_interpreter interpreterGuard_;
};

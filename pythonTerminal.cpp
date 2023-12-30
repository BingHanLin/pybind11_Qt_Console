

#include <QKeyEvent>

#include "pythonTerminal.hpp"

PYBIND11_EMBEDDED_MODULE(fast_calc, m) {
  pybind11::module_ cal = m.def_submodule("cal", "A submodule of 'fast_calc'");
  cal.def("add", [](int i, int j) { return i + j; });
}

pythonTerminalEdit::pythonTerminalEdit(QWidget *parent) : QTextEdit(parent) {

  pybind11::exec(R"(
        kwargs = dict(name="World", number=42)
        message = "Hello, {name}! The answer is {number}".format(**kwargs)
        print(message)
    )");

  auto fast_calc = pybind11::module_::import("fast_calc");

  auto locals = pybind11::dict(**fast_calc.attr("__dict__"));

  pybind11::exec(R"(
        print(cal.add(1, 2))
    )",
                 pybind11::globals(), locals);
}

void pythonTerminalEdit::runCommand() {}

void pythonTerminalEdit::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Return) {

    runCommand();
    event->accept();
    return;
  }

  QTextEdit::keyPressEvent(event);
}

#include <pybind11/pybind11.h>

#include "pythonCommands.hpp"

PYBIND11_EMBEDDED_MODULE(example, m) {
  m.doc() = "pybind11 example plugin"; // optional module docstring

  m.def("add", &add, "A function that adds two numbers");

  m.def("subtract", [](int i, int j) { return i - j; });
}
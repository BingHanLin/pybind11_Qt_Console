#pragma once

#pragma push_macro(                                                            \
    "slots") // solve slots variable name conflits between python and qt.
#undef slots
#include <pybind11/embed.h> // everything needed for embedding
#pragma pop_macro("slots")

PYBIND11_EMBEDDED_MODULE(fast_calc, m) {
  pybind11::module_ cal = m.def_submodule("cal", "A submodule of fast_calc'");
  cal.def("add", [](int i, int j) { return i + j; });
}
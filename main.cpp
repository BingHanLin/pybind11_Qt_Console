#include <pybind11/embed.h> // everything needed for embedding
namespace py = pybind11;

int main() {
  py::scoped_interpreter guard{}; // start the interpreter and keep it alive

  py::exec(R"(
        kwargs = dict(name="World", number=42)
        message = "Hello, {name}! The answer is {number}".format(**kwargs)
        print(message)
    )");

  py::print("Hello, World!"); // use the Python API
}
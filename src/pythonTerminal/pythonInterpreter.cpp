
#include "pythonInterpreter.hpp"
#include "pythonCommands.hpp"

pythonInterpreter::pythonInterpreter(std::shared_ptr<dataModel> model)
{
    pythonCommands::model_ = model;

    pybind11::exec(R"(
        kwargs = dict(name="World", number=42)
        message = "Hello, {name}! The answer is {number}".format(**kwargs)
        print(message)
    )");

    auto demo_commands = pybind11::module_::import("demo_commands");
    locals_ = pybind11::dict(**demo_commands.attr("__dict__"));
}

pythonInterpreter::~pythonInterpreter()
{
}

void pythonInterpreter::runCommand(const std::string& cmd) const
{
    pybind11::exec(cmd, pybind11::globals(), locals_);
};

#include <iostream>

#include <pybind11/stl.h>

#include "pythonCommands.hpp"
#include "pythonInterpreter.hpp"

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

    for (auto item : locals_)
    {
        std::cout << "key: " << item.first << ", value=" << item.second
                  << std::endl;
    };

    auto sub_commands =
        pybind11::module_::import("demo_commands.order_commands");
    auto sub_locals = pybind11::dict(**sub_commands.attr("__dict__"));

    for (auto item : sub_locals)
    {
        std::cout << "  key: " << item.first << ", value=" << item.second
                  << std::endl;
    };
}

pythonInterpreter::~pythonInterpreter()
{
}

void pythonInterpreter::runCommand(const std::string& cmd) const
{
    pybind11::exec(cmd, pybind11::globals(), locals_);
};

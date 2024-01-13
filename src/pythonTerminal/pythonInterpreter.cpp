#include <iostream>
#include <sstream>

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
}

pythonInterpreter::~pythonInterpreter()
{
}

void pythonInterpreter::runCommand(const std::string& cmd) const
{
    pybind11::exec(cmd, pybind11::globals(), locals_);
};

QStringList pythonInterpreter::getMethods(const QStringList& queryList) const
{
    QStringList fullQueryMethod({"demo_commands"});
    fullQueryMethod.append(queryList);
    fullQueryMethod.removeAll(QString(""));

    QStringList resultMethods;
    QStringList testQueryMethod;
    for (const auto oneQueryMethod : fullQueryMethod)
    {
        testQueryMethod.push_back(oneQueryMethod);
        const auto testQueryMethodString = testQueryMethod.join(".");

        try
        {
            auto sub_commands = pybind11::module_::import(
                testQueryMethodString.toStdString().c_str());
            auto sub_locals = pybind11::dict(**sub_commands.attr("__dict__"));

            resultMethods.clear();

            for (auto item : sub_locals)
            {
                std::stringstream ss;
                ss << item.first;
                const auto oneMethodName = QString::fromStdString(ss.str());

                if (!oneMethodName.startsWith("__"))
                {
                    resultMethods.push_back(oneMethodName);
                }
            };
        }
        catch (pybind11::error_already_set& e)
        {
            if (e.matches(PyExc_ModuleNotFoundError))
            {
                std::cout << "PyExc_ModuleNotFoundError" << std::endl;
            }

            std::cout << e.what() << std::endl;
        }
    }

    return resultMethods;
}

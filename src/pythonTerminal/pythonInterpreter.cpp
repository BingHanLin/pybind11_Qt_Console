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

std::vector<std::string> pythonInterpreter::getPossibleMethods(
    const std::vector<std::string>& queryVec) const
{
    std::vector<std::string> fullQueryVec{"demo_commands"};
    fullQueryVec.insert(fullQueryVec.end(), queryVec.begin(), queryVec.end());

    std::vector<std::string> results;
    std::vector<std::string> testQueryVec;
    int checkPassCounter = 0;
    for (const auto oneQuery : fullQueryVec)
    {
        testQueryVec.push_back(oneQuery);
        std::string testQueryMethodString;

        if (testQueryVec.size() > 1)
        {
            // Use std::accumulate to concatenate the strings with "."
            testQueryMethodString = std::accumulate(
                testQueryVec.begin(), testQueryVec.end(), std::string(),
                [](const std::string& a, const std::string& b) -> std::string
                { return a + (a.length() > 0 ? "." : "") + b; });
        }
        else
        {
            testQueryMethodString = testQueryVec[0];
        }

        try
        {
            auto subCommands =
                pybind11::module_::import(testQueryMethodString.c_str());
            auto sub_locals = pybind11::dict(**subCommands.attr("__dict__"));

            results.clear();
            for (auto item : sub_locals)
            {
                std::stringstream ss;
                ss << item.first;
                const std::string oneMethodName = ss.str();

                if (oneMethodName.find("__") != 0)
                {
                    results.push_back(oneMethodName);
                }
            };

            checkPassCounter++;
        }
        catch (pybind11::error_already_set& e)
        {
            if (e.matches(PyExc_ModuleNotFoundError))
            {
                std::cout << "PyExc_ModuleNotFoundError" << std::endl;
            }

            // std::cout << e.what() << std::endl;

            if (checkPassCounter < fullQueryVec.size() - 1)
            {
                results.clear();
            }
            break;
        }
    }

    return results;
}

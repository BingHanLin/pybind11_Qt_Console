#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

#include <pybind11/stl.h>

#include "pythonInterpreter.hpp"

std::vector<std::string> pythonInterpreter::getPossibleMethods(const std::vector<std::string>& queryVec)
{
    std::vector<std::string> fullQueryVec{"root_module"};
    fullQueryVec.insert(fullQueryVec.end(), queryVec.begin(), queryVec.end());

    std::vector<std::string> results;
    std::vector<std::string> testQueryVec;
    int checkPassCounter = 0;
    for (const auto& oneQuery : fullQueryVec)
    {
        testQueryVec.push_back(oneQuery);
        std::string testQueryMethodString;

        if (testQueryVec.size() > 1)
        {
            // Use std::accumulate to concatenate the strings with "."
            testQueryMethodString = std::accumulate(testQueryVec.begin(), testQueryVec.end(), std::string(),
                                                    [](const std::string& a, const std::string& b) -> std::string
                                                    { return a + (a.length() > 0 ? "." : "") + b; });
        }
        else
        {
            testQueryMethodString = testQueryVec[0];
        }

        try
        {
            {
                auto module = pybind11::module_::import(testQueryMethodString.c_str());
                // auto module = pybind11::module_::import("root_module.order_commands");
                auto object = pybind11::object(module.attr("__dict__"));

                const auto subModules = pybind11::eval(
                                            R"(
                            [name for name in dir() if __import__('importlib').import_module('inspect').ismodule(globals()[name])]
                          )",
                                            object)
                                            .cast<std::vector<std::string>>();

                const auto functions = pybind11::eval(
                                           R"(
                            [name for name in dir() if __import__('importlib').import_module('inspect').isroutine(globals()[name])]
                          )",
                                           object)
                                           .cast<std::vector<std::string>>();

                std::cout << "=========================" << std::endl;
                std::cout << "testQueryMethodString: " << testQueryMethodString << std::endl;
                if (!subModules.empty())
                {
                    std::cout << "module:" << std::endl;
                    for (const auto& submodule : subModules)
                    {
                        std::cout << ">>>>>" << std::endl;

                        std::cout << submodule << std::endl;

                        auto obj = module.attr(submodule.c_str());
                        std::cout << obj.doc().cast<std::string>() << std::endl;
                    };
                }

                if (!functions.empty())
                {
                    std::cout << "function:" << std::endl;
                    for (const auto& function : functions)
                    {
                        std::cout << ">>>>>" << std::endl;

                        std::cout << function << std::endl;

                        auto obj = module.attr(function.c_str());
                        std::cout << obj.doc().cast<std::string>() << std::endl;
                    };
                }
            }

            auto subCommands = pybind11::module_::import(testQueryMethodString.c_str());
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

pythonInterpreter::pythonInterpreter(QObject* parent) : QObject(parent)
{
    auto root_module = pybind11::module_::import("root_module");
    locals_ = root_module.attr("__dict__");
}

void pythonInterpreter::runCommand(const std::string& cmd) const
{
    try
    {
        emit pyCommandBeforeInserted(QString::fromStdString(cmd));

        pyStdErrOutStreamRedirect redirect;

        pybind11::exec(cmd, pybind11::globals(), locals_);

        emit pyCommandStdOutput(QString::fromStdString(redirect.stdoutString()));
    }
    catch (pybind11::error_already_set& e)
    {
        if (e.matches(PyExc_ModuleNotFoundError))
        {
            std::cout << "PyExc_ModuleNotFoundError" << std::endl;
        }

        emit pyCommandParsedWithError(QString::fromStdString(e.what()));

        std::cout << e.what() << std::endl;
    }
};

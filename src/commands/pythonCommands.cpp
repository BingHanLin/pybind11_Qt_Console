#include "pythonCommands.hpp"
#include "commandManager.hpp"
#include "commands.hpp"

std::shared_ptr<dataModel> pythonCommands::model_ = nullptr;

void pythonCommands::setDataModel(std::shared_ptr<dataModel> model)
{
    pythonCommands::model_ = std::move(model);
}

PYBIND11_EMBEDDED_MODULE(demo_commands, m)
{
    pybind11::module_ order_commands = m.def_submodule("order_commands", "A submodule of demo_commands'");

    order_commands.def(
        "add_order",
        [](int id, int amount, double price)
        {
            if (pythonCommands::model_ != nullptr)
            {
                auto newOrder = std::make_shared<order>(order{id, amount, price});

                auto command = new addCommand(pythonCommands::model_, newOrder);

                auto cmdManager = commandManager::getInstance();
                cmdManager->runCommand(command);
            }
        },
        R"mydelimiter(
    The add order function

    Parameters
    ----------
)mydelimiter");

    order_commands.def(
        "remove_order",
        [](int id)
        {
            if (pythonCommands::model_ != nullptr)
            {
                auto command = new removeCommand(pythonCommands::model_, id);

                auto cmdManager = commandManager::getInstance();
                cmdManager->runCommand(command);
            }
        },
        R"mydelimiter(
    The remove oder function

    Parameters
    ----------
)mydelimiter");

    order_commands.def("update_order",
                       [](int id, int amount, double price)
                       {
                           if (pythonCommands::model_ != nullptr)
                           {
                               auto command = new updateCommand(pythonCommands::model_, id, amount, price);

                               auto cmdManager = commandManager::getInstance();
                               cmdManager->runCommand(command);
                           }
                       });
}
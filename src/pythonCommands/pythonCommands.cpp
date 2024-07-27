#include <sstream>

#include "commandManager.hpp"
#include "commands.hpp"
#include "order.hpp"
#include "pythonCommands.hpp"

std::shared_ptr<dataModel> pythonCommands::model_ = nullptr;

void pythonCommands::setDataModel(std::shared_ptr<dataModel> model)
{
    pythonCommands::model_ = std::move(model);
}

PYBIND11_EMBEDDED_MODULE(demo_commands, m)
{
    {
        m.def(
            "redo", []() { commandManager::getInstance()->redo(); }, "Redo task.");
        commandManager::setRedoScript("redo()");
    }

    {
        m.def(
            "undo", []() { commandManager::getInstance()->undo(); }, "Undo task.");
        commandManager::setUndoScript("undo()");
    }

    pybind11::module_ order_commands = m.def_submodule("order_commands", "A submodule of demo_commands");

    pybind11::class_<order>(order_commands, "order")
        .def("__repr__",
             [](const order& o)
             {
                 return "<order: id " + std::to_string(o.id_) + ", amount " + std::to_string(o.amount_) + ", price " +
                        std::to_string(o.price_) + ">";
             });

    order_commands.def(
        "add_order",
        [](int id, int amount, double price) -> order*
        {
            if (pythonCommands::model_ != nullptr)
            {
                auto newOrder = std::make_shared<order>(id, amount, price);

                auto command = new addCommand(pythonCommands::model_, newOrder);

                auto cmdManager = commandManager::getInstance();
                cmdManager->runCommand(command);

                return pythonCommands::model_->getOrder(id).get();
            }

            return nullptr;
        },
        pybind11::return_value_policy::reference, "Add order with id and contents.", pybind11::arg("id"),
        pybind11::arg("amount"), pybind11::arg("price"));

    {
        addCommand::scriptCallbackType callback = [](int id, int amount, double price) -> std::string
        {
            std::ostringstream oss;
            oss << "order_commands.add_order(" << id << ", " << amount << ", " << price << ")";
            return oss.str();
        };

        addCommand::setScriptCallback(callback);
    };

    order_commands.def(
        "remove_order",
        [](int id) -> void
        {
            if (pythonCommands::model_ != nullptr)
            {
                auto command = new removeCommand(pythonCommands::model_, id);

                auto cmdManager = commandManager::getInstance();
                cmdManager->runCommand(command);
            }
        },
        "Remove order with id.", pybind11::arg("id"));

    {
        removeCommand::scriptCallbackType callback = [](int id) -> std::string
        {
            std::ostringstream oss;
            oss << "order_commands.remove_order(" << id << ")";
            return oss.str();
        };

        removeCommand::setScriptCallback(callback);
    };

    order_commands.def(
        "update_order",
        [](int id, int amount, double price) -> void
        {
            if (pythonCommands::model_ != nullptr)
            {
                auto command = new updateCommand(pythonCommands::model_, id, amount, price);

                auto cmdManager = commandManager::getInstance();
                cmdManager->runCommand(command);
            }
        },
        "Update order with id and contents.", pybind11::arg("id"), pybind11::arg("amount"), pybind11::arg("price"));

    {
        updateCommand::scriptCallbackType callback = [](int id, int amount, double price) -> std::string
        {
            std::ostringstream oss;
            oss << "order_commands.update_order(" << id << ", " << amount << ", " << price << ")";
            return oss.str();
        };

        updateCommand::setScriptCallback(callback);
    };
}
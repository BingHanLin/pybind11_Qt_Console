#include <memory>
#include <sstream>

#include "commandManager.hpp"
#include "commands.hpp"
#include "object.hpp"
#include "pythonCommandUtil.hpp"
#include "pythonCommands.hpp"
#include "root.hpp"

std::shared_ptr<dataModel> pythonCommands::model_ = nullptr;

void pythonCommands::setDataModel(std::shared_ptr<dataModel> model)
{
    pythonCommands::model_ = std::move(model);
}

PYBIND11_EMBEDDED_MODULE(root_module, root_var)
{
    {
        root_var.def(
            "redo", []() { commandManager::getInstance()->redo(); }, "Redo task.");
        commandManager::setRedoScript("redo()");
    }

    {
        root_var.def(
            "undo", []() { commandManager::getInstance()->undo(); }, "Undo task.");
        commandManager::setUndoScript("undo()");
    }

    {
        root_var.def(
            "model_root",
            []() -> root*
            {
                if (pythonCommands::model_ != nullptr)
                {
                    return pythonCommands::model_->getRoot().get();
                }

                return nullptr;
            },
            "Get model root.");
    }

    pybind11::class_<object>(root_var, "object")
        .def("__repr__", [](const object& o) { return "<object: name " + o.getName() + ">"; })
        .def("findChild", [](const object& o, const std::string& name) { return o.findChild(name).get(); });

    pybind11::module_ order_commands = root_var.def_submodule("order_commands", "A submodule of root_module");

    order_commands.def(
        "add_order",
        [](int id, int amount, double price) -> order*
        {
            if (pythonCommands::model_ != nullptr)
            {
                auto newOrder = std::make_shared<order>("New Order", id, amount, price);

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
        updateCommand::scriptCallbackType callback = [](const std::shared_ptr<order>& oneOrder, int amount,
                                                        double price) -> std::string
        {
            const auto script = getObjectScript(oneOrder);

            std::ostringstream oss;
            oss << script.findScript_ << "\n";
            oss << "order_commands.update_order(" << script.targetName_ << ", " << amount << ", " << price << ")";
            return oss.str();
        };

        updateCommand::setScriptCallback(callback);
    };
}
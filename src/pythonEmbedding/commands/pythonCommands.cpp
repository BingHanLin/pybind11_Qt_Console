#include <memory>
#include <sstream>

#include "commandManager.hpp"
#include "commands.hpp"
#include "object.hpp"
#include "order.hpp"
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
            []() -> std::shared_ptr<root>
            {
                if (pythonCommands::model_ != nullptr)
                {
                    return pythonCommands::model_->getRoot();
                }

                return nullptr;
            },
            pybind11::return_value_policy::reference, "Get model root.");
    }

    pybind11::class_<object, std::shared_ptr<object>>(root_var, "object")
        .def("__repr__", [](const object& o) { return "<object: name " + o.getName() + ">"; })
        .def(
            "findChild",
            [](const object& o, const std::string& name) -> std::shared_ptr<object> { return o.findChild(name); },
            pybind11::return_value_policy::reference, "Find child object by name.", pybind11::arg("name"));

    pybind11::class_<root, object, std::shared_ptr<root>>(root_var, "root")
        .def("__repr__", [](const object& o) { return "<root: name " + o.getName() + ">"; });

    pybind11::class_<group, object, std::shared_ptr<group>>(root_var, "group")
        .def("__repr__", [](const object& o) { return "<group: name " + o.getName() + ">"; });

    pybind11::class_<order, object, std::shared_ptr<order>>(root_var, "order")
        .def("__repr__", [](const object& o) { return "<order: name " + o.getName() + ">"; });

    pybind11::module_ order_commands = root_var.def_submodule("order_commands", "A submodule of root_module");

    order_commands.def(
        "add_order",
        [](const std::shared_ptr<group>& oneGroup, int amount, double price) -> std::shared_ptr<order>
        {
            if (pythonCommands::model_ != nullptr)
            {
                auto newOrder = std::make_shared<order>("New Order", amount, price, oneGroup.get());
                auto command = new addOrderCommand(pythonCommands::model_, oneGroup, newOrder);
                auto cmdManager = commandManager::getInstance();
                cmdManager->runCommand(command);

                return newOrder;
            }

            return nullptr;
        },
        pybind11::return_value_policy::reference, "Add order with id and contents.", pybind11::arg("id"),
        pybind11::arg("amount"), pybind11::arg("price"));

    {
        addOrderCommand::scriptCallbackType callback = [](const std::shared_ptr<group>& oneGroup, int amount,
                                                          double price) -> std::string
        {
            const auto groupScript = getObjectScript(oneGroup);

            std::ostringstream oss;
            oss << groupScript.findScript_ << "\n";
            oss << "order_commands.add_order(" << groupScript.targetName_ << ", " << amount << ", " << price << ")";
            return oss.str();
        };

        addOrderCommand::setScriptCallback(callback);
    };

    order_commands.def(
        "remove_order",
        [](const std::shared_ptr<group>& oneGroup, const std::shared_ptr<order>& oneOrder) -> void
        {
            if (pythonCommands::model_ != nullptr)
            {
                auto command = new removeOrderCommand(pythonCommands::model_, oneGroup, oneOrder);

                auto cmdManager = commandManager::getInstance();
                cmdManager->runCommand(command);
            }
        },
        "Remove order with id.", pybind11::arg("group"), pybind11::arg("order"));

    {
        removeOrderCommand::scriptCallbackType callback = [](const std::shared_ptr<group>& oneGroup,
                                                             const std::shared_ptr<order>& oneOrder) -> std::string
        {
            const auto groupScript = getObjectScript(oneGroup);
            const auto orderScript = getObjectScript(oneOrder);

            std::ostringstream oss;
            oss << groupScript.findScript_ << "\n";
            oss << orderScript.findScript_ << "\n";
            oss << "order_commands.remove_order(" << groupScript.targetName_ << ", " << orderScript.targetName_ << ")";
            return oss.str();
        };

        removeOrderCommand::setScriptCallback(callback);
    };

    order_commands.def(
        "update_order",
        [](const std::shared_ptr<order>& oneOrder, int amount, double price) -> void
        {
            if (pythonCommands::model_ != nullptr)
            {
                auto command = new updateOrderCommand(pythonCommands::model_, oneOrder, amount, price);

                auto cmdManager = commandManager::getInstance();
                cmdManager->runCommand(command);
            }
        },
        "Update order with id and contents.", pybind11::arg("order"), pybind11::arg("amount"), pybind11::arg("price"));

    {
        updateOrderCommand::scriptCallbackType callback = [](const std::shared_ptr<order>& oneOrder, int amount,
                                                             double price) -> std::string
        {
            const auto script = getObjectScript(oneOrder);

            std::ostringstream oss;
            oss << script.findScript_ << "\n";
            oss << "order_commands.update_order(" << script.targetName_ << ", " << amount << ", " << price << ")";
            return oss.str();
        };

        updateOrderCommand::setScriptCallback(callback);
    };

    order_commands.def(
        "clear_all_orders",
        []() -> void
        {
            if (pythonCommands::model_ != nullptr)
            {
                auto command = new clearAllOrdersCommand(pythonCommands::model_);

                auto cmdManager = commandManager::getInstance();
                cmdManager->runCommand(command);
            }
        },
        "Clear all orders.");

    {
        clearAllOrdersCommand::scriptCallbackType callback = []() -> std::string
        { return "order_commands.clear_all_orders()"; };

        clearAllOrdersCommand::setScriptCallback(callback);
    };
}
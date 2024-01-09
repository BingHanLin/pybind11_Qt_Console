#include "pythonCommands.hpp"

std::shared_ptr<dataModel> pythonCommands::model_ = nullptr;

PYBIND11_EMBEDDED_MODULE(demo_commands, m)
{
    pybind11::module_ order_commands =
        m.def_submodule("order_commands", "A submodule of demo_commands'");

    order_commands.def(
        "add_order",
        [](int id, int amount, double price)
        {
            auto newOrder = std::make_shared<order>(order{id, amount, price});

            addCommand cmd(pythonCommands::model_, newOrder);
            cmd.execute();
        });

    order_commands.def("remove_order",
                       [](int id)
                       {
                           removeCommand cmd(pythonCommands::model_, id);
                           cmd.execute();
                       });

    order_commands.def("update_order",
                       [](int id, int amount, double price)
                       {
                           updateCommand cmd(pythonCommands::model_, id, amount,
                                             price);
                           cmd.execute();
                       });
}
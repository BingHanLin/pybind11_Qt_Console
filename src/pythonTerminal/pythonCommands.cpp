#include "pythonCommands.hpp"

std::shared_ptr<dataModel> pythonCommands::model_ = nullptr;

PYBIND11_EMBEDDED_MODULE(demo_commands, m)
{
    pybind11::module_ order_commands =
        m.def_submodule("order_commands", "A submodule of demo_commands'");

    order_commands.def("add_order",
                       [](int amount, double price)
                       {
                           order newOrder{0, amount, price};

                           addCommand cmd(pythonCommands::model_, newOrder);
                           cmd.execute();
                       });
}
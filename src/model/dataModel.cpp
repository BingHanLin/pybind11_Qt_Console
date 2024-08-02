#include "dataModel.hpp"
#include <memory>

dataModel::dataModel() : root_(std::make_shared<root>())
{
    auto g1 = std::make_shared<group>("Group1", root_.get());
    root_->addGroup(g1);

    {
        auto o1 = std::make_shared<order>("Order1", 5, 10.0, g1.get());
        g1->addOrder(o1);

        auto o2 = std::make_shared<order>("Order2", 6, 12.0, g1.get());
        g1->addOrder(o2);
    }
}

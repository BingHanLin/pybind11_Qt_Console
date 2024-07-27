#include "dataModel.hpp"
#include <memory>

dataModel::dataModel()
{
    auto o1 = std::make_shared<order>(1, 5, 10.0);
    orders_.insert({o1->id_, o1});

    auto o2 = std::make_shared<order>(2, 6, 12.0);
    orders_.insert({o2->id_, o2});

    auto o3 = std::make_shared<order>(3, 9, 15.0);
    orders_.insert({o3->id_, o3});
}

bool dataModel::addOrder(const std::shared_ptr<order>& o)
{
    const auto orderExisted = orders_.count(o->id_) != 0;

    orders_.insert({o->id_, o});

    emit dataChanged();

    if (orderExisted)
    {
        emit messageEmerged(tr("Added order %1. Overwrite the old order.").arg(o->id_));
    }
    else
    {
        emit messageEmerged(tr("Added order %1.").arg(o->id_));
    }

    return true;
}

bool dataModel::removeOrder(const int id)
{
    if (orders_.count(id) != 0)
    {
        orders_.erase(id);

        emit dataChanged();
        emit messageEmerged(tr("Removed order %1.").arg(id));

        return true;
    }
    else
    {
        emit messageEmerged(tr("Remove order %1 failed. Order not found.").arg(id));
        return false;
    }
}

bool dataModel::updateOrder(const int& id, const int& amount, const double& price)
{
    if (orders_.count(id) != 0)
    {
        orders_.at(id)->amount_ = amount;
        orders_.at(id)->price_ = price;

        emit dataChanged();
        emit messageEmerged(tr("Updated order %1.").arg(id));

        return true;
    }
    else
    {
        emit messageEmerged(tr("Update order %1 failed. Order not found.").arg(id));
        return false;
    }
}

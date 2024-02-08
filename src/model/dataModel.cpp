#include "dataModel.hpp"
#include <memory>

dataModel::dataModel()
{
    auto o1 = std::make_shared<order>(order{1, 5, 10.0});
    orders_.insert({o1->id_, o1});

    auto o2 = std::make_shared<order>(order{2, 6, 12.0});
    orders_.insert({o2->id_, o2});

    auto o3 = std::make_shared<order>(order{3, 9, 15.0});
    orders_.insert({o3->id_, o3});
}

void dataModel::addOrder(const std::shared_ptr<order>& o)
{
    orders_.insert({o->id_, o});
    emit dataChanged();
}

void dataModel::removeOrder(const int id)
{
    if (orders_.count(id) != 0)
    {
        orders_.erase(id);
        emit dataChanged();
    }
    else
    {
    }
}

void dataModel::updateOrder(const int& id, const int& amount, const double& price)
{
    if (orders_.count(id) != 0)
    {
        orders_.at(id)->amount_ = amount;
        orders_.at(id)->price_ = price;

        emit dataChanged();
    }
    else
    {
    }
}

#include "dataModel.hpp"

dataModel::dataModel()
{
}

dataModel::~dataModel()
{
}

void dataModel::addOrder(const std::shared_ptr<order> o)
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
        /* v does not contain x */
    }
}

void dataModel::updateOrder(const int id, const int amount, const double price)
{
    if (orders_.count(id) != 0)
    {
        orders_.at(id)->amount_ = amount;
        orders_.at(id)->price_ = price;

        emit dataChanged();
    }
    else
    {
        /* v does not contain x */
    }
}

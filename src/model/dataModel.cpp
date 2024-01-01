#include "dataModel.hpp"

dataModel::dataModel()
{
}

dataModel::~dataModel()
{
}

void dataModel::addOrder(const order& o)
{
    orders_.push_back(o);
    emit dataChanged();
}

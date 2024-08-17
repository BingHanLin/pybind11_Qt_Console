#pragma once

#include "dataModel.hpp"
#include "nlohmann/json.hpp"

inline nlohmann::json getAllOrdersInfo(const std::shared_ptr<dataModel>& model)
{
    nlohmann::json result = nlohmann::json::array();
    auto groups = model->getRoot()->getGroups();
    for (const auto& group : groups)
    {
        nlohmann::json orders = nlohmann::json::array();
        for (const auto& order : group->getOrders())
        {
            nlohmann::json orderInfo = nlohmann::json::object();
            orderInfo["name"] = order->getName();
            orderInfo["price"] = order->getPrice();
            orderInfo["amount"] = order->getAmount();
            orders.push_back(orderInfo);
        }

        nlohmann::json groupInfo = nlohmann::json::object();
        groupInfo["group"] = group->getName();
        groupInfo["orders"] = orders;

        result.push_back(groupInfo);
    }

    return result;
}

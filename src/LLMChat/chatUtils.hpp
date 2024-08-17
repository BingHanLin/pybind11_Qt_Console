#pragma once

#include <memory>

#include "commandManager.hpp"
#include "commands.hpp"
#include "dataModel.hpp"
#include "nlohmann/json.hpp"
#include "order.hpp"

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

inline bool deleteOrder(const std::shared_ptr<dataModel>& model, const std::string& group, const std::string& name)
{
    const auto groups = model->getRoot()->getGroups();

    // find the group
    auto groupIter =
        std::find_if(groups.begin(), groups.end(), [group](const auto& g) { return g->getName() == group; });
    if (groupIter == groups.end())
    {
        return false;
    }

    const auto orders = (*groupIter)->getOrders();

    // find the order
    auto orderIter = std::find_if(orders.begin(), orders.end(), [name](const auto& o) { return o->getName() == name; });

    if (orderIter == orders.end())
    {
        return false;
    }

    auto command = new removeOrderCommand(model, (*groupIter), (*orderIter));
    auto cmdManager = commandManager::getInstance();
    cmdManager->runCommand(command);

    return true;
}

inline bool addOrder(const std::shared_ptr<dataModel>& model, const std::string& group, const std::string& name,
                     const double price, const int amount)
{
    const auto groups = model->getRoot()->getGroups();

    // find the group
    auto groupIter =
        std::find_if(groups.begin(), groups.end(), [group](const auto& g) { return g->getName() == group; });
    if (groupIter == groups.end())
    {
        return false;
    }

    auto newOrder = std::make_shared<order>(name, price, amount, (*groupIter).get());

    auto command = new addOrderCommand(model, (*groupIter), newOrder);
    auto cmdManager = commandManager::getInstance();
    cmdManager->runCommand(command);

    return true;
}

inline bool clearAllOrders(const std::shared_ptr<dataModel>& model)
{
    auto command = new clearAllOrdersCommand(model);
    auto cmdManager = commandManager::getInstance();
    cmdManager->runCommand(command);

    return true;
}

#pragma once

#include "object.hpp"
#include "order.hpp"

class group : public object
{
  public:
    explicit group(const std::string& name, object* parent) : object(name, parent){};
    ~group() = default;

    [[nodiscard]] std::vector<std::shared_ptr<object>> getChildren() const override
    {
        std::vector<std::shared_ptr<object>> results;
        results.reserve(orders_.size());
        for (const auto& order : orders_)
        {
            results.push_back(order);
        }

        return results;
    };

    std::vector<std::shared_ptr<order>> getOrders()
    {
        return orders_;
    }

    void addOrder(const std::shared_ptr<order>& o)
    {
        orders_.push_back(o);
    }

    void removeOrder(const std::shared_ptr<order>& o)
    {
        orders_.erase(std::remove(orders_.begin(), orders_.end(), o), orders_.end());
    }

  private:
    std::vector<std::shared_ptr<order>> orders_;
};

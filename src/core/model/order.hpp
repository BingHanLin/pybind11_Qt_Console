#pragma once

#include "object.hpp"

class order : public object
{
  public:
    order(const std::string& name, int amount, double price, object* parent)
        : object(name, parent), amount_(amount), price_(price){};

    ~order()
    {
        std::cout << "Order has been destroyed." << std::endl;
    };

    [[nodiscard]] std::vector<std::shared_ptr<object>> getChildren() const override
    {
        return {};
    };

    [[nodiscard]] int getAmount() const
    {
        return amount_;
    }

    [[nodiscard]] double getPrice() const
    {
        return price_;
    }

    int amount_;
    double price_;
};
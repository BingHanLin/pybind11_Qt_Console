#pragma once

#include <iostream>

struct order
{
    order(int id, int amount, double price) : id_(id), amount_(amount), price_(price){};

    ~order()
    {
        std::cout << "Order with id: " << id_ << " has been destroyed." << std::endl;
    };

    int id_;
    int amount_;
    double price_;
};
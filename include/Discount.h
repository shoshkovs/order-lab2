#pragma once

#include <string>

class Order;

class Discount {
public:
    virtual ~Discount() = default;

    virtual void apply(Order& order) const = 0;
    virtual std::string getName() const = 0;
};

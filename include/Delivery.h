#pragma once

#include <memory>
#include <string>

class Order;

class Delivery : public std::enable_shared_from_this<Delivery> {
public:
    virtual ~Delivery() = default;

    virtual double getCost(const Order& order) const = 0;
    virtual std::string getEstimatedTime() const = 0;
    virtual bool isAvailable(const Order& order) const = 0;
    virtual std::string getName() const = 0;
};

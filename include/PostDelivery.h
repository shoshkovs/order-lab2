#pragma once

#include "Delivery.h"

class PostDelivery : public Delivery {
public:
    double getCost(const Order& order) const override;
    std::string getEstimatedTime() const override;
    bool isAvailable(const Order& order) const override;
    std::string getName() const override;

private:
    static constexpr double cost_ = 200.0;
    static constexpr double maxWeight_ = 10.0;
};

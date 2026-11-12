#pragma once

#include "Delivery.h"

class PickupDelivery : public Delivery {
public:
    double getCost(const Order& order) const override;
    std::string getEstimatedTime() const override;
    bool isAvailable(const Order& order) const override;
    std::string getName() const override;
};

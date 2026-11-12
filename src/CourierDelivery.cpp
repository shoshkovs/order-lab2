#include "CourierDelivery.h"

#include "Order.h"

#include <stdexcept>

double CourierDelivery::getCost(const Order& order) const {
    if (!isAvailable(order)) {
        throw std::invalid_argument("Courier: order is heavier than 20 kg");
    }
    return cost_;
}

std::string CourierDelivery::getEstimatedTime() const {
    return "1-2 days";
}

bool CourierDelivery::isAvailable(const Order& order) const {
    return order.getTotalWeight() <= maxWeight_;
}

std::string CourierDelivery::getName() const {
    return "Courier";
}

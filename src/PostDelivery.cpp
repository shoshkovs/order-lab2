#include "PostDelivery.h"

#include "Order.h"

#include <stdexcept>

double PostDelivery::getCost(const Order& order) const {
    if (!isAvailable(order)) {
        throw std::invalid_argument("Post: order is heavier than 10 kg");
    }
    return cost_;
}

std::string PostDelivery::getEstimatedTime() const {
    return "5-10 days";
}

bool PostDelivery::isAvailable(const Order& order) const {
    return order.getTotalWeight() <= maxWeight_;
}

std::string PostDelivery::getName() const {
    return "Post";
}

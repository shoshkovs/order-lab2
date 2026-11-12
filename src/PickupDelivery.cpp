#include "PickupDelivery.h"

#include "Order.h"

double PickupDelivery::getCost(const Order&) const {
    return 0.0;
}

std::string PickupDelivery::getEstimatedTime() const {
    return "1-3 days";
}

bool PickupDelivery::isAvailable(const Order&) const {
    // у самовывоза нет ограничения по весу
    return true;
}

std::string PickupDelivery::getName() const {
    return "Pickup";
}

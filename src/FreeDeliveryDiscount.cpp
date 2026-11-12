#include "FreeDeliveryDiscount.h"

#include "Order.h"

void FreeDeliveryDiscount::apply(Order& order) const {
    order.setFreeDelivery(true);
}

std::string FreeDeliveryDiscount::getName() const {
    return "Free delivery";
}

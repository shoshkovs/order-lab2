#include "Operators.h"

Order& operator+(Order& order, const Discount& discount) {
    order.applyDiscount(discount);
    return order;
}

Order& operator<<(Delivery& delivery, Order& order) {
    // доставка хранится в shared_ptr, поэтому берем общий указатель на нее
    order.setDelivery(delivery.shared_from_this());
    return order;
}

std::ostream& operator<<(std::ostream& output, const Order& order) {
    output << "Order " << order.getId() << " | " << order.getCustomer()
           << " | " << toString(order.getStatus())
           << " | items=" << order.getItems().size()
           << " | weight=" << order.getTotalWeight()
           << " | goods=" << order.getItemsPrice();
    if (order.getDelivery() != nullptr) {
        output << " | " << order.getDelivery()->getName()
               << " " << order.getDeliveryCost();
    }
    output << " | total=" << order.getTotalPrice()
           << " | bonus=" << order.getBonus();
    return output;
}

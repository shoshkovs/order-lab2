#include "OrderEvent.h"

OrderEvent::OrderEvent(int orderId, OrderStatus oldStatus, OrderStatus newStatus)
    : orderId_(orderId),
      oldStatus_(oldStatus),
      newStatus_(newStatus),
      timestamp_(std::chrono::system_clock::now()) {}

int OrderEvent::getOrderId() const {
    return orderId_;
}

OrderStatus OrderEvent::getOldStatus() const {
    return oldStatus_;
}

OrderStatus OrderEvent::getNewStatus() const {
    return newStatus_;
}

std::chrono::system_clock::time_point OrderEvent::getTimestamp() const {
    return timestamp_;
}

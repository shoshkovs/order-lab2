#pragma once

#include "OrderStatus.h"

#include <chrono>

class OrderEvent {
public:
    OrderEvent(int orderId, OrderStatus oldStatus, OrderStatus newStatus);

    int getOrderId() const;
    OrderStatus getOldStatus() const;
    OrderStatus getNewStatus() const;
    std::chrono::system_clock::time_point getTimestamp() const;

private:
    int orderId_;
    OrderStatus oldStatus_;
    OrderStatus newStatus_;
    std::chrono::system_clock::time_point timestamp_;
};

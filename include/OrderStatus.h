#pragma once

#include <string>

enum class OrderStatus {
    Pending,
    Paid,
    Shipped,
    Delivered,
    Cancelled
};

std::string toString(OrderStatus status);

#pragma once

#include "Delivery.h"
#include "Discount.h"
#include "Order.h"

#include <ostream>

Order& operator+(Order& order, const Discount& discount);
Order& operator<<(Delivery& delivery, Order& order);
std::ostream& operator<<(std::ostream& output, const Order& order);

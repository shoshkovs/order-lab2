#include "PercentDiscount.h"

#include "Order.h"

#include <stdexcept>

PercentDiscount::PercentDiscount(double percent) : percent_(percent) {
    if (percent <= 0.0 || percent >= 100.0) {
        throw std::invalid_argument("Percent must be between 0 and 100");
    }
}

void PercentDiscount::apply(Order& order) const {
    for (const std::shared_ptr<Product>& item : order.getItems()) {
        item->setCurrentPrice(item->getCurrentPrice() * (100.0 - percent_) / 100.0);
    }
}

std::string PercentDiscount::getName() const {
    return "Percent " + std::to_string(static_cast<int>(percent_)) + "%";
}

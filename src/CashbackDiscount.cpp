#include "CashbackDiscount.h"

#include "Order.h"

#include <stdexcept>

CashbackDiscount::CashbackDiscount(double percent) : percent_(percent) {
    if (percent <= 0.0 || percent >= 100.0) {
        throw std::invalid_argument("Percent must be between 0 and 100");
    }
}

void CashbackDiscount::apply(Order& order) const {
    // цена не меняется, процент от товаров идет на бонусный счет
    order.addBonus(order.getItemsPrice() * percent_ / 100.0);
}

std::string CashbackDiscount::getName() const {
    return "Cashback " + std::to_string(static_cast<int>(percent_)) + "%";
}

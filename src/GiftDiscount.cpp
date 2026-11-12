#include "GiftDiscount.h"

#include "Order.h"
#include "ProductType.h"

#include <memory>

GiftDiscount::GiftDiscount(int giftId, const std::string& category)
    : giftId_(giftId), category_(category) {}

void GiftDiscount::apply(Order& order) const {
    std::shared_ptr<Product> gift =
        std::make_shared<Product>(giftId_, ProductType::getType(category_), 1);
    gift->setCurrentPrice(0.0);
    order.addItem(gift);
}

std::string GiftDiscount::getName() const {
    return "Gift " + category_;
}

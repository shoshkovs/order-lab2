#pragma once

#include "Discount.h"

#include <string>

class GiftDiscount : public Discount {
public:
    GiftDiscount(int giftId, const std::string& category);

    void apply(Order& order) const override;
    std::string getName() const override;

private:
    int giftId_;
    std::string category_;
};

#pragma once

#include "Discount.h"

class FreeDeliveryDiscount : public Discount {
public:
    void apply(Order& order) const override;
    std::string getName() const override;
};

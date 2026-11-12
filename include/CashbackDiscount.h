#pragma once

#include "Discount.h"

class CashbackDiscount : public Discount {
public:
    explicit CashbackDiscount(double percent);

    void apply(Order& order) const override;
    std::string getName() const override;

private:
    double percent_;
};

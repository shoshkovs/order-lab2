#pragma once

#include "Discount.h"

class PercentDiscount : public Discount {
public:
    explicit PercentDiscount(double percent);

    void apply(Order& order) const override;
    std::string getName() const override;

private:
    double percent_;
};

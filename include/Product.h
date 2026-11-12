#pragma once

#include "ProductType.h"

#include <memory>

class Product {
public:
    Product(int id, const std::shared_ptr<ProductType>& type, int quantity);

    int getId() const;
    int getQuantity() const;
    double getCurrentPrice() const;
    const ProductType& getType() const;

    double getWeight() const;
    double getTotalPrice() const;
    void changeQuantity(int delta);
    void setCurrentPrice(double price);

private:
    int id_;
    int quantity_;
    double currentPrice_;
    std::shared_ptr<ProductType> type_;
};

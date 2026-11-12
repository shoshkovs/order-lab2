#include "Product.h"

#include <stdexcept>

Product::Product(int id, const std::shared_ptr<ProductType>& type, int quantity)
    : id_(id), quantity_(quantity), currentPrice_(0.0), type_(type) {
    if (id <= 0) {
        throw std::invalid_argument("Product ID must be positive");
    }
    if (type == nullptr) {
        throw std::invalid_argument("Product type cannot be null");
    }
    if (quantity <= 0) {
        throw std::invalid_argument("Quantity must be positive");
    }
    currentPrice_ = type->getBasePrice();
}

int Product::getId() const {
    return id_;
}

int Product::getQuantity() const {
    return quantity_;
}

double Product::getCurrentPrice() const {
    return currentPrice_;
}

const ProductType& Product::getType() const {
    return *type_;
}

double Product::getWeight() const {
    return type_->getBaseWeight() * quantity_;
}

double Product::getTotalPrice() const {
    return currentPrice_ * quantity_;
}

void Product::changeQuantity(int delta) {
    if (quantity_ + delta <= 0) {
        throw std::invalid_argument("Quantity must stay positive");
    }
    quantity_ += delta;
}

void Product::setCurrentPrice(double price) {
    if (price < 0.0) {
        throw std::invalid_argument("Price cannot be negative");
    }
    currentPrice_ = price;
}

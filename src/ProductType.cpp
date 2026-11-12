#include "ProductType.h"

#include <stdexcept>

std::unordered_map<std::string, std::shared_ptr<ProductType>> ProductType::pool_;

ProductType::ProductType(const std::string& category,
                         double baseWeight,
                         double basePrice)
    : category_(category), baseWeight_(baseWeight), basePrice_(basePrice) {
    if (category.empty()) {
        throw std::invalid_argument("Category cannot be empty");
    }
    if (baseWeight < 0.0 || basePrice < 0.0) {
        throw std::invalid_argument("Weight and price cannot be negative");
    }
}

std::shared_ptr<ProductType> ProductType::getType(const std::string& category) {
    auto found = pool_.find(category);
    if (found == pool_.end()) {
        throw std::out_of_range("Unknown product type: " + category);
    }
    return found->second;
}

std::shared_ptr<ProductType> ProductType::getType(const std::string& category,
                                                  double baseWeight,
                                                  double basePrice) {
    auto found = pool_.find(category);
    if (found != pool_.end()) {
        // общий объект уже есть, новый не создаем
        return found->second;
    }
    std::shared_ptr<ProductType> type =
        std::make_shared<ProductType>(category, baseWeight, basePrice);
    pool_[category] = type;
    return type;
}

std::size_t ProductType::getPoolSize() {
    return pool_.size();
}

const std::string& ProductType::getCategory() const {
    return category_;
}

double ProductType::getBaseWeight() const {
    return baseWeight_;
}

double ProductType::getBasePrice() const {
    return basePrice_;
}

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

class ProductType {
public:
    ProductType(const std::string& category, double baseWeight, double basePrice);

    static std::shared_ptr<ProductType> getType(const std::string& category);
    static std::shared_ptr<ProductType> getType(const std::string& category,
                                                double baseWeight,
                                                double basePrice);
    static std::size_t getPoolSize();

    const std::string& getCategory() const;
    double getBaseWeight() const;
    double getBasePrice() const;

private:
    static std::unordered_map<std::string, std::shared_ptr<ProductType>> pool_;

    std::string category_;
    double baseWeight_;
    double basePrice_;
};
